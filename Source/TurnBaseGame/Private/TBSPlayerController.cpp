#include "TBSPlayerController.h"
#include "Containers/Map.h"
#include "TBSGridManager.h"
#include "TBSCell.h"
#include "TBSUnit.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "EngineUtils.h"
#include "InputCoreTypes.h"

// Costruttore del PlayerController
ATBSPlayerController::ATBSPlayerController()
{
	// Attiva il Tick del PlayerController
	PrimaryActorTick.bCanEverTick = true;

	// Mostra il cursore del mouse
	bShowMouseCursor = true;

	// Abilita eventi di click e hover
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// All'inizio nessuna cella è selezionata
	CurrentlySelectedCell = nullptr;

	// All'inizio nessuna unità è selezionata
	CurrentlySelectedUnit = nullptr;
}

void ATBSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Imposto un input mode misto: il gioco continua a ricevere input,
	// ma posso anche usare il mouse nell'interfaccia
	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);

	// Cerco una CameraActor presente nel livello
	// e la imposto come camera principale del giocatore
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		// Uso la prima camera trovata nel livello
		SetViewTarget(*It);
		break;
	}
}

void ATBSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// Se il tasto sinistro del mouse viene premuto in questo frame,
	// gestisco il click manualmente
	if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		OnLeftMouseClick();
	}
}

void ATBSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ATBSPlayerController::OnLeftMouseClick()
{
	FHitResult HitResult;

	// Faccio un raycast sotto il cursore del mouse
	bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	// Se il raycast ha colpito qualcosa e l'attore esiste, continuo
	if (bHit && HitResult.GetActor())
	{
		// Prima controllo se ho cliccato un'unità
		ATBSUnit* ClickedUnit = Cast<ATBSUnit>(HitResult.GetActor());

		if (ClickedUnit)
		{
			// Cerco il GridManager presente nel livello
			ATBSGridManager* GridManager = nullptr;

			for (TActorIterator<ATBSGridManager> It(GetWorld()); It; ++It)
			{
				GridManager = *It;
				break;
			}

			// Se non trovo il GridManager, blocco tutto per sicurezza
			if (!GridManager)
			{
				UE_LOG(LogTemp, Warning, TEXT("GridManager non trovato."));
				return;
			}

			// Caso 1: ho cliccato una unita umana -> selezione
			if (GridManager->HumanUnits.Contains(ClickedUnit))
			{
				// Se era gia selezionata un'altra unita, tolgo la selezione visiva
				if (CurrentlySelectedUnit && CurrentlySelectedUnit != ClickedUnit)
				{
					CurrentlySelectedUnit->SetSelected(false);
				}

				// Salvo la nuova unita come unita attualmente selezionata
				CurrentlySelectedUnit = ClickedUnit;

				// Applico la selezione visiva alla nuova unita
				CurrentlySelectedUnit->SetSelected(true);

				// Mostro il range di movimento dell'unita selezionata
				ShowMovementRange(CurrentlySelectedUnit);

				// Mostro anche i bersagli attaccabili dell'unita selezionata
				ShowAttackRange(CurrentlySelectedUnit);

				// Scrivo nel log la posizione logica dell'unita selezionata
				UE_LOG(LogTemp, Warning, TEXT("Unita umana selezionata -> X: %d | Y: %d"), ClickedUnit->GridX, ClickedUnit->GridY);

				return;
			}

			// Caso 2: ho cliccato una unita AI e ho una mia unita selezionata -> provo ad attaccare
			if (GridManager->AIUnits.Contains(ClickedUnit))
			{
				// Se non ho nessuna unita selezionata, non posso attaccare
				if (!CurrentlySelectedUnit)
				{
					UE_LOG(LogTemp, Warning, TEXT("Seleziona prima una tua unita."));
					return;
				}

				// Controllo se il bersaglio e nel range di attacco
				if (!IsEnemyInAttackRange(CurrentlySelectedUnit, ClickedUnit))
				{
					UE_LOG(LogTemp, Warning, TEXT("Bersaglio fuori dal range di attacco."));
					return;
				}

				// Calcolo il danno random dell'attaccante
				const int32 DamageDealt = CurrentlySelectedUnit->RollDamage();

				// Applico il danno al bersaglio
				ClickedUnit->ReceiveDamage(DamageDealt);

				UE_LOG(LogTemp, Warning, TEXT("Attacco eseguito: danno inflitto = %d"), DamageDealt);

				// Nascondo il range di movimento dopo l'azione
				HideMovementRange();

				// Nascondo il range di attacco dopo l'azione
				HideAttackRange();

				// Tolgo selezione visiva all'unita del player
				CurrentlySelectedUnit->SetSelected(false);

				// Deseleziono l'unita del player dopo l'attacco
				CurrentlySelectedUnit = nullptr;

				return;
			}

			// Se non rientra nei casi previsti, non faccio nulla
			return;
		}

		// Se non ho cliccato un'unità, provo a vedere se ho cliccato una cella
		ATBSCell* ClickedCell = Cast<ATBSCell>(HitResult.GetActor());

		if (ClickedCell)
		{
			// Se c'era una cella selezionata prima e non è la stessa,
			// tolgo la selezione visiva dalla vecchia cella
			if (CurrentlySelectedCell && CurrentlySelectedCell != ClickedCell)
			{
				CurrentlySelectedCell->SetSelected(false);
			}

			// Salvo la nuova cella come selezionata
			CurrentlySelectedCell = ClickedCell;

			// Applico il materiale di selezione alla nuova cella
			CurrentlySelectedCell->SetSelected(true);

			// Scrivo nel log le coordinate della cella cliccata
			UE_LOG(LogTemp, Warning, TEXT("Cella cliccata -> X: %d | Y: %d"), ClickedCell->GridX, ClickedCell->GridY);

			// Se ho un'unità selezionata, provo a spostarla sulla cella cliccata
			if (CurrentlySelectedUnit)
			{
				// Prima controllo se la cella è attraversabile
				if (!ClickedCell->bIsWalkable)
				{
					// Se non è attraversabile, blocco il movimento
					UE_LOG(LogTemp, Warning, TEXT("Movimento bloccato: cella non attraversabile."));
					return;
				}

				// Controllo se la cella cliccata rientra nel range di movimento attualmente evidenziato
				if (!HighlightedMovementCells.Contains(ClickedCell))
				{
					// Se la cella non è nel range, blocco il movimento
					UE_LOG(LogTemp, Warning, TEXT("Movimento bloccato: cella fuori dal range di movimento."));
					return;
				}

				// Calcolo il percorso minimo verso la cella cliccata
				TArray<ATBSCell*> PathCells;
				bool bPathFound = FindPathToCell(CurrentlySelectedUnit, ClickedCell, PathCells);

				// Se non trovo un percorso valido, blocco il movimento
				if (!bPathFound || PathCells.Num() <= 1)
				{
					UE_LOG(LogTemp, Warning, TEXT("Movimento bloccato: nessun percorso valido."));
					return;
				}

				// Converto il percorso di celle in posizioni mondo
				TArray<FVector> WorldPath;
				BuildWorldPathFromCells(PathCells, WorldPath);

				// Avvio il movimento lungo il percorso
				CurrentlySelectedUnit->StartPathMovement(ClickedCell->GridX, ClickedCell->GridY, WorldPath);
				
				// Nascondo il range di movimento dopo aver effettuato l'azione
				HideMovementRange();

				// Tolgo la selezione visiva dall'unità dopo il movimento
				CurrentlySelectedUnit->SetSelected(false);

				// Dopo il movimento, deseleziono l'unità
				CurrentlySelectedUnit = nullptr;

				// Scrivo nel log che il movimento è terminato
				UE_LOG(LogTemp, Warning, TEXT("Movimento completato: unita deselezionata."));
			}
		}
	}
}

// Nasconde il range di movimento attualmente mostrato
void ATBSPlayerController::HideMovementRange()
{
	for (ATBSCell* Cell : HighlightedMovementCells)
	{
		if (IsValid(Cell))
		{
			Cell->SetSelected(false);
		}
	}

	HighlightedMovementCells.Empty();
}

// Mostra il range di movimento dell'unità selezionata
void ATBSPlayerController::ShowMovementRange(ATBSUnit* Unit)
{
	// Se l'unità non è valida, esco
	if (!Unit)
	{
		return;
	}

	// Prima pulisco eventuale range precedente
	HideMovementRange();

	// Calcolo tutte le celle realmente raggiungibili
	TArray<ATBSCell*> ReachableCells;
	ComputeReachableCells(Unit, ReachableCells);

	// Evidenzio le celle raggiungibili
	for (ATBSCell* Cell : ReachableCells)
	{
		if (IsValid(Cell))
		{
			Cell->SetSelected(true);
			HighlightedMovementCells.Add(Cell);
		}
	}
}

// Restituisce il GridManager presente nel livello
ATBSGridManager* ATBSPlayerController::GetGridManager() const
{
	for (TActorIterator<ATBSGridManager> It(GetWorld()); It; ++It)
	{
		return *It;
	}

	return nullptr;
}

// Cerca una cella tramite coordinate di griglia
ATBSCell* ATBSPlayerController::GetCellAtCoordinates(int32 X, int32 Y) const
{
	ATBSGridManager* GridManager = GetGridManager();

	if (!GridManager)
	{
		return nullptr;
	}

	for (ATBSCell* Cell : GridManager->SpawnedCells)
	{
		if (IsValid(Cell) && Cell->GridX == X && Cell->GridY == Y)
		{
			return Cell;
		}
	}

	return nullptr;
}

// Controlla se una cella è occupata da un'altra unità
bool ATBSPlayerController::IsCellOccupied(int32 X, int32 Y, const ATBSUnit* IgnoredUnit) const
{
	ATBSGridManager* GridManager = GetGridManager();

	if (!GridManager)
	{
		return false;
	}

	// Controllo unità umane
	for (ATBSUnit* Unit : GridManager->HumanUnits)
	{
		if (!IsValid(Unit) || Unit == IgnoredUnit)
		{
			continue;
		}

		if (Unit->GridX == X && Unit->GridY == Y)
		{
			return true;
		}
	}

	// Controllo unità AI
	for (ATBSUnit* Unit : GridManager->AIUnits)
	{
		if (!IsValid(Unit) || Unit == IgnoredUnit)
		{
			continue;
		}

		if (Unit->GridX == X && Unit->GridY == Y)
		{
			return true;
		}
	}

	return false;
}

// Calcola tutte le celle raggiungibili da una unità in base al costo del terreno
void ATBSPlayerController::ComputeReachableCells(ATBSUnit* Unit, TArray<ATBSCell*>& OutReachableCells) const
{
	if (!Unit)
	{
		return;
	}

	ATBSGridManager* GridManager = GetGridManager();

	if (!GridManager)
	{
		return;
	}

	// Costo minimo trovato per ogni coordinata
	TMap<FIntPoint, int32> BestCostByCell;

	// Coda semplice di esplorazione
	TArray<FIntPoint> OpenList;
	OpenList.Add(FIntPoint(Unit->GridX, Unit->GridY));
	BestCostByCell.Add(FIntPoint(Unit->GridX, Unit->GridY), 0);

	// Movimento massimo dell'unità
	const int32 MaxMovement = Unit->GetMovementRange();

	// Direzioni ortogonali: destra, sinistra, alto, basso
	const TArray<FIntPoint> Directions =
	{
		FIntPoint(1, 0),
		FIntPoint(-1, 0),
		FIntPoint(0, 1),
		FIntPoint(0, -1)
	};

	// Esplorazione stile Dijkstra semplificata
	while (OpenList.Num() > 0)
	{
		// Estraggo la prima cella da esplorare
		FIntPoint CurrentCoords = OpenList[0];
		OpenList.RemoveAt(0);

		ATBSCell* CurrentCell = GetCellAtCoordinates(CurrentCoords.X, CurrentCoords.Y);
		if (!CurrentCell)
		{
			continue;
		}

		const int32 CurrentCost = BestCostByCell[CurrentCoords];

		// Provo le 4 direzioni ortogonali
		for (const FIntPoint& Direction : Directions)
		{
			FIntPoint NextCoords = CurrentCoords + Direction;

			ATBSCell* NextCell = GetCellAtCoordinates(NextCoords.X, NextCoords.Y);
			if (!NextCell)
			{
				continue;
			}

			// Non posso passare su celle non attraversabili
			if (!NextCell->bIsWalkable)
			{
				continue;
			}

			// Non posso passare sopra celle occupate da altre unità
			if (IsCellOccupied(NextCell->GridX, NextCell->GridY, Unit))
			{
				continue;
			}

			// Calcolo il costo del passo in base al dislivello
			int32 StepCost = 1;

			if (NextCell->HeightLevel > CurrentCell->HeightLevel)
			{
				// Movimento in salita
				StepCost = 2;
			}
			else
			{
				// Movimento in piano o in discesa
				StepCost = 1;
			}

			const int32 NewCost = CurrentCost + StepCost;

			// Se sforo il range massimo, scarto
			if (NewCost > MaxMovement)
			{
				continue;
			}

			// Se non avevo mai visto questa cella o ho trovato un costo migliore, aggiorno
			if (!BestCostByCell.Contains(NextCoords) || NewCost < BestCostByCell[NextCoords])
			{
				BestCostByCell.Add(NextCoords, NewCost);
				OpenList.Add(NextCoords);

				// Aggiungo la cella alle raggiungibili, evitando duplicati
				if (!OutReachableCells.Contains(NextCell))
				{
					OutReachableCells.Add(NextCell);
				}
			}
		}
	}
}

// Calcola il percorso minimo tra l'unità e una cella destinazione
bool ATBSPlayerController::FindPathToCell(ATBSUnit* Unit, ATBSCell* DestinationCell, TArray<ATBSCell*>& OutPathCells) const
{
	if (!Unit || !DestinationCell)
	{
		return false;
	}

	ATBSGridManager* GridManager = GetGridManager();
	if (!GridManager)
	{
		return false;
	}

	// Nodo iniziale
	FIntPoint Start(Unit->GridX, Unit->GridY);
	FIntPoint Goal(DestinationCell->GridX, DestinationCell->GridY);

	// Costo minimo per coordinata
	TMap<FIntPoint, int32> BestCostByCell;

	// Coordinata precedente per ricostruire il percorso
	TMap<FIntPoint, FIntPoint> PreviousCell;

	// Lista aperta
	TArray<FIntPoint> OpenList;
	OpenList.Add(Start);
	BestCostByCell.Add(Start, 0);

	// Direzioni ortogonali
	const TArray<FIntPoint> Directions =
	{
		FIntPoint(1, 0),
		FIntPoint(-1, 0),
		FIntPoint(0, 1),
		FIntPoint(0, -1)
	};

	const int32 MaxMovement = Unit->GetMovementRange();

	while (OpenList.Num() > 0)
	{
		// Scelgo il nodo con costo minore
		int32 BestIndex = 0;
		int32 BestCost = TNumericLimits<int32>::Max();

		for (int32 i = 0; i < OpenList.Num(); i++)
		{
			const int32 Cost = BestCostByCell[OpenList[i]];
			if (Cost < BestCost)
			{
				BestCost = Cost;
				BestIndex = i;
			}
		}

		FIntPoint CurrentCoords = OpenList[BestIndex];
		OpenList.RemoveAt(BestIndex);

		// Se sono arrivato a destinazione, ricostruisco il percorso
		if (CurrentCoords == Goal)
		{
			TArray<FIntPoint> ReversePath;
			FIntPoint Step = Goal;
			ReversePath.Add(Step);

			while (PreviousCell.Contains(Step))
			{
				Step = PreviousCell[Step];
				ReversePath.Add(Step);
			}

			// Inverto per ottenere Start -> Goal
			for (int32 i = ReversePath.Num() - 1; i >= 0; i--)
			{
				ATBSCell* PathCell = GetCellAtCoordinates(ReversePath[i].X, ReversePath[i].Y);
				if (PathCell)
				{
					OutPathCells.Add(PathCell);
				}
			}

			return true;
		}

		ATBSCell* CurrentCell = GetCellAtCoordinates(CurrentCoords.X, CurrentCoords.Y);
		if (!CurrentCell)
		{
			continue;
		}

		for (const FIntPoint& Direction : Directions)
		{
			FIntPoint NextCoords = CurrentCoords + Direction;

			ATBSCell* NextCell = GetCellAtCoordinates(NextCoords.X, NextCoords.Y);
			if (!NextCell)
			{
				continue;
			}

			// Non posso attraversare celle non walkable
			if (!NextCell->bIsWalkable)
			{
				continue;
			}

			// Non posso attraversare celle occupate da altre unità
			if (IsCellOccupied(NextCell->GridX, NextCell->GridY, Unit))
			{
				continue;
			}

			// Costo del passo
			int32 StepCost = 1;
			if (NextCell->HeightLevel > CurrentCell->HeightLevel)
			{
				StepCost = 2;
			}

			const int32 NewCost = BestCostByCell[CurrentCoords] + StepCost;

			// Se sforo il movimento massimo, scarto
			if (NewCost > MaxMovement)
			{
				continue;
			}

			// Aggiorno se trovo un percorso migliore
			if (!BestCostByCell.Contains(NextCoords) || NewCost < BestCostByCell[NextCoords])
			{
				BestCostByCell.Add(NextCoords, NewCost);
				PreviousCell.Add(NextCoords, CurrentCoords);

				if (!OpenList.Contains(NextCoords))
				{
					OpenList.Add(NextCoords);
				}
			}
		}
	}

	return false;
}

// Converte un percorso di celle in posizioni mondo
void ATBSPlayerController::BuildWorldPathFromCells(const TArray<ATBSCell*>& PathCells, TArray<FVector>& OutWorldPath) const
{
	OutWorldPath.Empty();

	// Salto la prima cella del percorso, perché è la cella attuale dell'unità
	for (int32 i = 1; i < PathCells.Num(); i++)
	{
		if (!IsValid(PathCells[i]))
		{
			continue;
		}

		FVector WorldLocation = PathCells[i]->GetActorLocation();
		WorldLocation.Z += 60.0f;
		OutWorldPath.Add(WorldLocation);
	}
}

// Controlla se un bersaglio nemico è nel range di attacco dell'unità selezionata
bool ATBSPlayerController::IsEnemyInAttackRange(ATBSUnit* Attacker, ATBSUnit* Target) const
{
	if (!Attacker || !Target)
	{
		return false;
	}

	// Distanza Manhattan sulla griglia
	const int32 Distance =
		FMath::Abs(Target->GridX - Attacker->GridX) +
		FMath::Abs(Target->GridY - Attacker->GridY);

	return Distance <= Attacker->GetAttackRange();
}

// Nasconde i bersagli attaccabili attualmente mostrati
void ATBSPlayerController::HideAttackRange()
{
	for (ATBSCell* Cell : HighlightedAttackCells)
	{
		if (IsValid(Cell))
		{
			Cell->SetAttackHighlighted(false);
		}
	}

	HighlightedAttackCells.Empty();
}

// Mostra i bersagli attaccabili dell'unita selezionata
void ATBSPlayerController::ShowAttackRange(ATBSUnit* Unit)
{
	if (!Unit)
	{
		return;
	}

	// Pulisco eventuale evidenziazione precedente
	HideAttackRange();

	ATBSGridManager* GridManager = GetGridManager();
	if (!GridManager)
	{
		return;
	}

	// Controllo tutte le unita AI come potenziali bersagli
	for (ATBSUnit* EnemyUnit : GridManager->AIUnits)
	{
		if (!IsValid(EnemyUnit))
		{
			continue;
		}

		// Se il bersaglio è nel range di attacco, evidenzio la sua cella
		if (IsEnemyInAttackRange(Unit, EnemyUnit))
		{
			ATBSCell* EnemyCell = GetCellAtCoordinates(EnemyUnit->GridX, EnemyUnit->GridY);

			if (EnemyCell)
			{
				EnemyCell->SetAttackHighlighted(true);
				HighlightedAttackCells.Add(EnemyCell);
			}
		}
	}
}