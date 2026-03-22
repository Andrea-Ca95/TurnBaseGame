#include "TBSCell.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Costruttore della cella
ATBSCell::ATBSCell()
{
	// La cella non ha bisogno di Tick
	PrimaryActorTick.bCanEverTick = false;

	// Creo il componente mesh della cella
	CellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CellMesh"));

	// Imposto la mesh come root dell'attore
	RootComponent = CellMesh;

	// Carico il cubo base di Unreal come mesh della cella
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Se la mesh è stata trovata correttamente, la assegno
	if (CubeMeshAsset.Succeeded())
	{
		CellMesh->SetStaticMesh(CubeMeshAsset.Object);
	}

	// Rendo la tile leggermente più bassa in Z e un po' più piccola in X/Y
	SetActorScale3D(FVector(0.95f, 0.95f, 0.1f));

	// Valori iniziali logici
	GridX = 0;
	GridY = 0;
	HeightLevel = 1;
	bIsWalkable = true;

	// All'inizio la cella non è selezionata
	bIsSelected = false;

	// I materiali verranno assegnati dal GridManager
	BaseTerrainMaterial = nullptr;
	SelectedMaterial = nullptr;
}

// Funzione chiamata quando la cella entra nel mondo
void ATBSCell::BeginPlay()
{
	Super::BeginPlay();

	// Applico il materiale base del terreno all'avvio, se disponibile
	if (CellMesh && BaseTerrainMaterial)
	{
		CellMesh->SetMaterial(0, BaseTerrainMaterial);
	}
}

// Applica o rimuove la selezione visiva della cella
void ATBSCell::SetSelected(bool bSelected)
{
	// Salvo lo stato interno di selezione
	bIsSelected = bSelected;

	// Se per qualche motivo la mesh non esiste, esco
	if (!CellMesh)
	{
		return;
	}

	// Se la cella è selezionata e ho un materiale di selezione, lo applico
	if (bIsSelected && SelectedMaterial)
	{
		CellMesh->SetMaterial(0, SelectedMaterial);
	}
	// Altrimenti torno al materiale base del terreno
	else if (BaseTerrainMaterial)
	{
		CellMesh->SetMaterial(0, BaseTerrainMaterial);
	}
}

// Aggiorna posizione e scala della cella in base al livello di altezza
void ATBSCell::UpdateVisualFromHeight(float CellSize)
{
	// Recupero la posizione attuale della cella
	FVector CurrentLocation = GetActorLocation();

	// Calcolo una quota Z reale a partire dal livello logico
	float HeightStep = CellSize * 0.2f;

	// Imposto l'altezza nel mondo in base al livello della cella
	CurrentLocation.Z = HeightLevel * HeightStep;

	// Applico la nuova posizione
	SetActorLocation(CurrentLocation);

	// Mantengo la cella leggermente bassa ma con spessore sufficiente per leggere i dislivelli
	SetActorScale3D(FVector(0.95f, 0.95f, 0.2f));

	// Se ho un materiale terreno valido e la cella non è selezionata, lo applico
	if (CellMesh && BaseTerrainMaterial && !bIsSelected)
	{
		CellMesh->SetMaterial(0, BaseTerrainMaterial);
	}
}