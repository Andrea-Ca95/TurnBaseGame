#include "TBSGridManager.h"
#include "TBSCell.h"
#include "TBSUnit.h"
#include "Engine/World.h"

// Costruttore del GridManager
ATBSGridManager::ATBSGridManager()
{
	// Non serve il Tick: il manager non deve aggiornarsi ogni frame
	PrimaryActorTick.bCanEverTick = false;

	// Dimensioni iniziali della griglia
	GridWidth = 25;
	GridHeight = 25;

	// Dimensione di ogni cella
	CellSize = 100.0f;

	// Usiamo direttamente la classe C++ della cella
	CellClass = ATBSCell::StaticClass();

	// All'inizio i materiali non sono assegnati
	DefaultCellMaterial = nullptr;
	SelectedCellMaterial = nullptr;

	// Usiamo direttamente la classe C++ dell'unità
	UnitClass = ATBSUnit::StaticClass();

	// Coordinate iniziali dell'unità sulla griglia
	InitialUnitX = 2;
	InitialUnitY = 2;

	// All'inizio non esiste ancora nessuna unità spawnata
	SpawnedUnit = nullptr;
}

// Quando il gioco inizia
void ATBSGridManager::BeginPlay()
{
	Super::BeginPlay();

	// Rigenera la griglia all'avvio del gioco
	GenerateGrid();

	// Crea l'unità iniziale solo durante il gioco
	SpawnInitialUnit();
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

	// Punto di origine della griglia: la posizione del GridManager nel mondo
	FVector Origin = GetActorLocation();

	// Ciclo su tutte le coordinate Y e X della griglia
	for (int32 Y = 0; Y < GridHeight; Y++)
	{
		for (int32 X = 0; X < GridWidth; X++)
		{
			// Calcolo la posizione nel mondo della cella corrente
			FVector SpawnLocation = Origin + FVector(X * CellSize, Y * CellSize, 0.0f);

			// Creo la cella nel mondo
			ATBSCell* NewCell = GetWorld()->SpawnActor<ATBSCell>(CellClass, SpawnLocation, FRotator::ZeroRotator);

			// Se la cella è stata creata correttamente, le assegno i dati
			if (NewCell)
			{
				// Coordinate logiche della cella nella griglia
				NewCell->GridX = X;
				NewCell->GridY = Y;

				// Altezza base della cella
				NewCell->HeightLevel = 1;

				// Di default la cella è attraversabile
				NewCell->bIsWalkable = true;

				// Esempio di ostacolo fisso per test:
				// la cella in posizione (4, 2) non sarà attraversabile
				if (X == 4 && Y == 2)
				{
					NewCell->bIsWalkable = false;
				}

				// Assegno i materiali alla cella
				NewCell->DefaultMaterial = DefaultCellMaterial;
				NewCell->SelectedMaterial = SelectedCellMaterial;

				// Salvo la cella nell'array
				SpawnedCells.Add(NewCell);
			}
		}
	}

	// Messaggio di debug nel log per confermare quante celle sono state create
	UE_LOG(LogTemp, Warning, TEXT("Griglia creata: %d celle"), SpawnedCells.Num());
}

// Funzione che crea l'unità iniziale solo durante il gioco
void ATBSGridManager::SpawnInitialUnit()
{
	// Se la classe dell'unità non è valida, esco
	if (!UnitClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnitClass non assegnata."));
		return;
	}

	// Se esiste già una unità spawnata, la distruggo prima di crearne una nuova
	if (IsValid(SpawnedUnit))
	{
		SpawnedUnit->Destroy();
		SpawnedUnit = nullptr;
	}

	// Calcolo la posizione iniziale dell'unità nel mondo
	FVector Origin = GetActorLocation();
	FVector UnitSpawnLocation = Origin + FVector(InitialUnitX * CellSize, InitialUnitY * CellSize, 60.0f);

	// Creo l'unità nel mondo
	ATBSUnit* NewUnit = GetWorld()->SpawnActor<ATBSUnit>(UnitClass, UnitSpawnLocation, FRotator::ZeroRotator);

	// Se l'unità è stata creata correttamente, salvo riferimento e coordinate logiche
	if (NewUnit)
	{
		NewUnit->GridX = InitialUnitX;
		NewUnit->GridY = InitialUnitY;
		SpawnedUnit = NewUnit;
	}
}