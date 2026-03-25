#include "TBSGridManager.h"
#include "TBSSniper.h"
#include "TBSBrawler.h"
#include "Engine/World.h"

// Costruttore del GridManager
ATBSGridManager::ATBSGridManager()
{
	// Non serve il Tick: il manager non deve aggiornarsi ogni frame
	PrimaryActorTick.bCanEverTick = false;

	// Dimensioni iniziali della griglia da specifica
	GridWidth = 25;
	GridHeight = 25;

	// Dimensione di ogni cella
	CellSize = 100.0f;

	// Usiamo direttamente la classe C++ della cella
	CellClass = ATBSCell::StaticClass();

	// Usiamo direttamente la classe C++ della torre
	TowerClass = ATBSTower::StaticClass();

	// All'inizio i materiali non sono assegnati
	SelectedCellMaterial = nullptr;
	Level0Material = nullptr;
	Level1Material = nullptr;
	Level2Material = nullptr;
	Level3Material = nullptr;
	Level4Material = nullptr;
	AttackRangeCellMaterial = nullptr;

	// All'inizio i materiali delle unità non sono assegnati
	HumanUnitMaterial = nullptr;
	AIUnitMaterial = nullptr;

	// Usiamo direttamente la classe C++ dell'unità
	UnitClass = ATBSUnit::StaticClass();

	// Usiamo direttamente le classi C++ delle unità concrete
	SniperClass = ATBSSniper::StaticClass();
	BrawlerClass = ATBSBrawler::StaticClass();

	// Parametri iniziali di generazione mappa
	// -1 significa: genera un seed casuale a runtime
	MapSeed = -1;

	// Scala del Perlin Noise
	NoiseScale = 5.0f;

	// Da specifica: livelli da 0 a 4
	MaxHeightLevel = 4;
}

// Quando il gioco inizia
void ATBSGridManager::BeginPlay()
{
	Super::BeginPlay();

	// Rigenera la griglia all'avvio del gioco
	GenerateGrid();

	// Crea le torri dopo la generazione della mappa
	SpawnTowers();

	// Crea l'unità iniziale solo durante il gioco
	SpawnInitialUnits();
}

void ATBSGridManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Svuoto gli array di attori/oggetti gestiti
	SpawnedCells.Empty();
	SpawnedTowers.Empty();
	HumanUnits.Empty();
	AIUnits.Empty();

	Super::EndPlay(EndPlayReason);
}

// Funzione che crea tutte le celle della griglia
void ATBSGridManager::GenerateGrid()
{
	// Se la classe della cella non è valida, esco e scrivo un warning nel log
	if (!CellClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CellClass non assegnata."));
		return;
	}

	// Prima elimino eventuali celle già presenti, così evito duplicati
	for (ATBSCell* Cell : SpawnedCells)
	{
		if (IsValid(Cell))
		{
			Cell->Destroy();
		}
	}

	// Svuoto l'array delle celle spawnate
	SpawnedCells.Empty();

	// Se il seed è -1, ne genero uno casuale
	// Questo rende la mappa diversa a ogni esecuzione
	int32 EffectiveSeed = MapSeed;
	if (EffectiveSeed < 0)
	{
		EffectiveSeed = FMath::RandRange(0, 1000000);
	}

	// Punto di origine della griglia: la posizione del GridManager nel mondo
	FVector Origin = GetActorLocation();

	// Ciclo su tutte le coordinate Y e X della griglia
	for (int32 Y = 0; Y < GridHeight; Y++)
	{
		for (int32 X = 0; X < GridWidth; X++)
		{
			// Calcolo una coordinata normalizzata per il Perlin Noise
			float NoiseX = (static_cast<float>(X) + EffectiveSeed) / NoiseScale;
			float NoiseY = (static_cast<float>(Y) + EffectiveSeed) / NoiseScale;

			// Perlin Noise restituisce un valore tipicamente tra -1 e 1
			float NoiseValue = FMath::PerlinNoise2D(FVector2D(NoiseX, NoiseY));

			// Rimappo il valore da [-1, 1] a [0, 1]
			float NormalizedNoise = (NoiseValue + 1.0f) * 0.5f;

			// Converto il noise normalizzato in livelli discreti usando soglie manuali
			// Questo mi permette di controllare meglio quanta acqua e quante zone alte compaiono
			int32 GeneratedHeightLevel = 0;

			if (NormalizedNoise < 0.28f)
			{
				// Livello 0 = acqua
				GeneratedHeightLevel = 0;
			}
			else if (NormalizedNoise < 0.46f)
			{
				// Livello 1
				GeneratedHeightLevel = 1;
			}
			else if (NormalizedNoise < 0.64f)
			{
				// Livello 2
				GeneratedHeightLevel = 2;
			}
			else if (NormalizedNoise < 0.82f)
			{
				// Livello 3
				GeneratedHeightLevel = 3;
			}
			else
			{
				// Livello 4
				GeneratedHeightLevel = 4;
			}

			// Calcolo la posizione base della cella nel mondo
			FVector SpawnLocation = Origin + FVector(X * CellSize, Y * CellSize, 0.0f);

			// Creo la cella nel mondo
			ATBSCell* NewCell = GetWorld()->SpawnActor<ATBSCell>(CellClass, SpawnLocation, FRotator::ZeroRotator);

			// Se la cella è stata creata correttamente, le assegno i dati
			if (NewCell)
			{
				// Coordinate logiche della cella nella griglia
				NewCell->GridX = X;
				NewCell->GridY = Y;

				// Altezza generata della cella
				NewCell->HeightLevel = GeneratedHeightLevel;

				// Di default la cella è attraversabile
				NewCell->bIsWalkable = true;

				// Il livello 0 rappresenta l'acqua ed è non calpestabile
				if (NewCell->HeightLevel == 0)
				{
					NewCell->bIsWalkable = false;
				}

				// Assegno il materiale di selezione
				NewCell->SelectedMaterial = SelectedCellMaterial;

				// Assegno il materiale per i bersagli attaccabili
				NewCell->AttackRangeMaterial = AttackRangeCellMaterial;

				// Assegno il materiale base in base al livello di altezza
				switch (NewCell->HeightLevel)
				{
				case 0:
					NewCell->BaseTerrainMaterial = Level0Material;
					break;

				case 1:
					NewCell->BaseTerrainMaterial = Level1Material;
					break;

				case 2:
					NewCell->BaseTerrainMaterial = Level2Material;
					break;

				case 3:
					NewCell->BaseTerrainMaterial = Level3Material;
					break;

				case 4:
					NewCell->BaseTerrainMaterial = Level4Material;
					break;

				default:
					NewCell->BaseTerrainMaterial = Level1Material;
					break;
				}

				// Aggiorno l'aspetto visivo della cella in base all'altezza
				NewCell->UpdateVisualFromHeight(CellSize);

				// Salvo la cella nell'array
				SpawnedCells.Add(NewCell);
			}
		}
	}

	// Messaggio di debug nel log per confermare quante celle sono state create
	UE_LOG(LogTemp, Warning, TEXT("Griglia creata: %d celle | Seed usato: %d"), SpawnedCells.Num(), EffectiveSeed);
}

// Cerca la cella valida più vicina a una coordinata ideale per piazzare una torre
ATBSCell* ATBSGridManager::FindBestTowerCell(int32 IdealX, int32 IdealY) const
{
	ATBSCell* BestCell = nullptr;
	float BestDistance = TNumericLimits<float>::Max();

	// Controllo tutte le celle generate
	for (ATBSCell* Cell : SpawnedCells)
	{
		// Se la cella non è valida, la salto
		if (!IsValid(Cell))
		{
			continue;
		}

		// Una torre non può stare sull'acqua, quindi la cella deve essere walkable
		if (!Cell->bIsWalkable)
		{
			continue;
		}

		// Calcolo la distanza dalla posizione ideale
		float Distance = FVector2D::Distance(
			FVector2D(Cell->GridX, Cell->GridY),
			FVector2D(IdealX, IdealY)
		);

		// Tengo la cella valida più vicina alla posizione ideale
		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			BestCell = Cell;
		}
	}

	return BestCell;
}

// Crea le 3 torri sulla mappa
void ATBSGridManager::SpawnTowers()
{
	// Se la classe torre non è valida, esco
	if (!TowerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("TowerClass non assegnata."));
		return;
	}

	// Distruggo eventuali torri già presenti
	for (ATBSTower* Tower : SpawnedTowers)
	{
		if (IsValid(Tower))
		{
			Tower->Destroy();
		}
	}

	// Svuoto l'array delle torri spawnate
	SpawnedTowers.Empty();

	// Coordinate ideali delle 3 torri
	TArray<FIntPoint> IdealTowerPositions;
	IdealTowerPositions.Add(FIntPoint(12, 12));
	IdealTowerPositions.Add(FIntPoint(5, 12));
	IdealTowerPositions.Add(FIntPoint(19, 12));

	// Per ogni posizione ideale cerco una cella valida vicina
	for (const FIntPoint& IdealPos : IdealTowerPositions)
	{
		ATBSCell* TowerCell = FindBestTowerCell(IdealPos.X, IdealPos.Y);

		// Se non trovo una cella valida, salto questa torre
		if (!TowerCell)
		{
			UE_LOG(LogTemp, Warning, TEXT("Nessuna cella valida trovata per torre vicino a (%d, %d)."), IdealPos.X, IdealPos.Y);
			continue;
		}

		// La cella della torre diventa un ostacolo fisico non calpestabile
		TowerCell->bIsWalkable = false;

		// Calcolo la posizione della torre nel mondo
		FVector TowerLocation = TowerCell->GetActorLocation();

		// Alzo la torre sopra la cella
		TowerLocation.Z += 100.0f;

		// Creo la torre nel mondo
		ATBSTower* NewTower = GetWorld()->SpawnActor<ATBSTower>(TowerClass, TowerLocation, FRotator::ZeroRotator);

		// Se la torre è stata creata correttamente, salvo coordinate e riferimento
		if (NewTower)
		{
			NewTower->GridX = TowerCell->GridX;
			NewTower->GridY = TowerCell->GridY;

			SpawnedTowers.Add(NewTower);

			UE_LOG(LogTemp, Warning, TEXT("Torre creata in (%d, %d)."), NewTower->GridX, NewTower->GridY);
		}
	}
}

// Cerca una cella valida casuale nella zona del player umano (Y = 0, 1, 2)
ATBSCell* ATBSGridManager::FindRandomValidHumanSpawnCell(const TArray<ATBSCell*>& ReservedCells) const
{
	// Array temporaneo di tutte le celle valide per lo spawn umano
	TArray<ATBSCell*> ValidCells;

	for (ATBSCell* Cell : SpawnedCells)
	{
		if (!IsValid(Cell))
		{
			continue;
		}

		// Il player umano può spawnare solo nelle prime 3 righe
		if (Cell->GridY < 0 || Cell->GridY > 2)
		{
			continue;
		}

		// La cella deve essere calpestabile
		if (!Cell->bIsWalkable)
		{
			continue;
		}

		// Evito celle già riservate
		if (ReservedCells.Contains(Cell))
		{
			continue;
		}

		ValidCells.Add(Cell);
	}

	// Se non esistono celle valide, ritorno nullptr
	if (ValidCells.Num() == 0)
	{
		return nullptr;
	}

	// Scelgo una cella valida casuale
	int32 RandomIndex = FMath::RandRange(0, ValidCells.Num() - 1);
	return ValidCells[RandomIndex];
}

// Cerca una cella valida casuale nella zona AI (Y = 22, 23, 24)
ATBSCell* ATBSGridManager::FindRandomValidAISpawnCell(const TArray<ATBSCell*>& ReservedCells) const
{
	// Array temporaneo di tutte le celle valide per lo spawn AI
	TArray<ATBSCell*> ValidCells;

	for (ATBSCell* Cell : SpawnedCells)
	{
		if (!IsValid(Cell))
		{
			continue;
		}

		// L'AI può spawnare solo nelle ultime 3 righe
		if (Cell->GridY < 22 || Cell->GridY > 24)
		{
			continue;
		}

		// La cella deve essere calpestabile
		if (!Cell->bIsWalkable)
		{
			continue;
		}

		// Evito celle già riservate
		if (ReservedCells.Contains(Cell))
		{
			continue;
		}

		ValidCells.Add(Cell);
	}

	// Se non esistono celle valide, ritorno nullptr
	if (ValidCells.Num() == 0)
	{
		return nullptr;
	}

	// Scelgo una cella valida casuale
	int32 RandomIndex = FMath::RandRange(0, ValidCells.Num() - 1);
	return ValidCells[RandomIndex];
}

// Crea le unità iniziali nelle zone di schieramento
void ATBSGridManager::SpawnInitialUnits()
{
	// Pulisco eventuali unità umane precedenti
	for (ATBSUnit* Unit : HumanUnits)
	{
		if (IsValid(Unit))
		{
			Unit->Destroy();
		}
	}
	HumanUnits.Empty();

	// Pulisco eventuali unità AI precedenti
	for (ATBSUnit* Unit : AIUnits)
	{
		if (IsValid(Unit))
		{
			Unit->Destroy();
		}
	}
	AIUnits.Empty();

	// Verifico che le classi delle unità esistano
	if (!SniperClass || !BrawlerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SniperClass o BrawlerClass non assegnate."));
		return;
	}

	// Celle già riservate per evitare overlap
	TArray<ATBSCell*> ReservedCells;

	// --- HUMAN SNIPER ---
	ATBSCell* HumanSniperCell = FindRandomValidHumanSpawnCell(ReservedCells);
	if (HumanSniperCell)
	{
		FVector SpawnLocation = HumanSniperCell->GetActorLocation();
		SpawnLocation.Z += 60.0f;

		ATBSSniper* HumanSniper = GetWorld()->SpawnActor<ATBSSniper>(SniperClass, SpawnLocation, FRotator::ZeroRotator);
		if (HumanSniper)
		{
			HumanSniper->GridX = HumanSniperCell->GridX;
			HumanSniper->GridY = HumanSniperCell->GridY;
			// Assegno il materiale del team umano
			if (HumanUnitMaterial && HumanSniper->UnitMesh)
			{
				HumanSniper->UnitMesh->SetMaterial(0, HumanUnitMaterial);
			}
			HumanUnits.Add(HumanSniper);
			ReservedCells.Add(HumanSniperCell);
		}
	}

	// --- HUMAN BRAWLER ---
	ATBSCell* HumanBrawlerCell = FindRandomValidHumanSpawnCell(ReservedCells);

	if (HumanBrawlerCell)
	{
		FVector SpawnLocation = HumanBrawlerCell->GetActorLocation();
		SpawnLocation.Z += 60.0f;

		ATBSBrawler* HumanBrawler = GetWorld()->SpawnActor<ATBSBrawler>(BrawlerClass, SpawnLocation, FRotator::ZeroRotator);
		if (HumanBrawler)
		{
			HumanBrawler->GridX = HumanBrawlerCell->GridX;
			HumanBrawler->GridY = HumanBrawlerCell->GridY;
			// Assegno il materiale del team umano
			if (HumanUnitMaterial && HumanBrawler->UnitMesh)
			{
				HumanBrawler->UnitMesh->SetMaterial(0, HumanUnitMaterial);
			}
			HumanUnits.Add(HumanBrawler);
			ReservedCells.Add(HumanBrawlerCell);
		}
	}

	// --- AI SNIPER ---
	ATBSCell* AISniperCell = FindRandomValidAISpawnCell(ReservedCells);
	if (AISniperCell)
	{
		FVector SpawnLocation = AISniperCell->GetActorLocation();
		SpawnLocation.Z += 60.0f;

		ATBSSniper* AISniper = GetWorld()->SpawnActor<ATBSSniper>(SniperClass, SpawnLocation, FRotator::ZeroRotator);
		if (AISniper)
		{
			AISniper->GridX = AISniperCell->GridX;
			AISniper->GridY = AISniperCell->GridY;
			// Assegno il materiale del team AI
			if (AIUnitMaterial && AISniper->UnitMesh)
			{
				AISniper->UnitMesh->SetMaterial(0, AIUnitMaterial);
			}
			AIUnits.Add(AISniper);
			ReservedCells.Add(AISniperCell);
		}
	}

	// --- AI BRAWLER ---
	ATBSCell* AIBrawlerCell = FindRandomValidAISpawnCell(ReservedCells);
	if (AIBrawlerCell)
	{
		FVector SpawnLocation = AIBrawlerCell->GetActorLocation();
		SpawnLocation.Z += 60.0f;

		ATBSBrawler* AIBrawler = GetWorld()->SpawnActor<ATBSBrawler>(BrawlerClass, SpawnLocation, FRotator::ZeroRotator);
		if (AIBrawler)
		{
			AIBrawler->GridX = AIBrawlerCell->GridX;
			AIBrawler->GridY = AIBrawlerCell->GridY;
			// Assegno il materiale del team AI
			if (AIUnitMaterial && AIBrawler->UnitMesh)
			{
				AIBrawler->UnitMesh->SetMaterial(0, AIUnitMaterial);
			}
			AIUnits.Add(AIBrawler);
			ReservedCells.Add(AIBrawlerCell);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Unita iniziali create: Human=%d | AI=%d"), HumanUnits.Num(), AIUnits.Num());
}

// Conta quante unità umane sono nella zona di cattura di una torre
int32 ATBSGridManager::CountHumanUnitsInTowerZone(ATBSTower* Tower) const
{
	if (!Tower)
	{
		return 0;
	}

	int32 Count = 0;

	for (ATBSUnit* Unit : HumanUnits)
	{
		if (!IsValid(Unit))
		{
			continue;
		}

		// Distanza "anche in diagonale" -> Chebyshev distance
		const int32 DeltaX = FMath::Abs(Unit->GridX - Tower->GridX);
		const int32 DeltaY = FMath::Abs(Unit->GridY - Tower->GridY);
		const int32 Distance = FMath::Max(DeltaX, DeltaY);

		if (Distance <= 2)
		{
			Count++;
		}
	}

	return Count;
}

// Conta quante unità AI sono nella zona di cattura di una torre
int32 ATBSGridManager::CountAIUnitsInTowerZone(ATBSTower* Tower) const
{
	if (!Tower)
	{
		return 0;
	}

	int32 Count = 0;

	for (ATBSUnit* Unit : AIUnits)
	{
		if (!IsValid(Unit))
		{
			continue;
		}

		// Distanza "anche in diagonale" -> Chebyshev distance
		const int32 DeltaX = FMath::Abs(Unit->GridX - Tower->GridX);
		const int32 DeltaY = FMath::Abs(Unit->GridY - Tower->GridY);
		const int32 Distance = FMath::Max(DeltaX, DeltaY);

		if (Distance <= 2)
		{
			Count++;
		}
	}

	return Count;
}

// Aggiorna lo stato di controllo di tutte le torri
void ATBSGridManager::UpdateTowerControlStates()
{
	for (ATBSTower* Tower : SpawnedTowers)
	{
		if (!IsValid(Tower))
		{
			continue;
		}

		const int32 HumanCount = CountHumanUnitsInTowerZone(Tower);
		const int32 AICount = CountAIUnitsInTowerZone(Tower);

		// Caso 1: nessuna unità nella zona -> neutrale
		if (HumanCount == 0 && AICount == 0)
		{
			Tower->SetNeutral();
		}
		// Caso 2: presenti unità di entrambi -> contesa
		else if (HumanCount > 0 && AICount > 0)
		{
			Tower->SetContested();
		}
		// Caso 3: solo unità umane -> controllata dal player umano
		else if (HumanCount > 0 && AICount == 0)
		{
			Tower->SetControlled(ETBSPlayerOwner::Human);
		}
		// Caso 4: solo unità AI -> controllata dalla AI
		else if (AICount > 0 && HumanCount == 0)
		{
			Tower->SetControlled(ETBSPlayerOwner::AI);
		}
	}
}

// Rimuove dagli array le unità non più valide o distrutte
void ATBSGridManager::CleanupDestroyedUnits()
{
	// Rimuovo le unità umane non più valide
	for (int32 i = HumanUnits.Num() - 1; i >= 0; i--)
	{
		if (!IsValid(HumanUnits[i]))
		{
			HumanUnits.RemoveAt(i);
		}
	}

	// Rimuovo le unità AI non più valide
	for (int32 i = AIUnits.Num() - 1; i >= 0; i--)
	{
		if (!IsValid(AIUnits[i]))
		{
			AIUnits.RemoveAt(i);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Cleanup unita -> Human: %d | AI: %d"), HumanUnits.Num(), AIUnits.Num());
}