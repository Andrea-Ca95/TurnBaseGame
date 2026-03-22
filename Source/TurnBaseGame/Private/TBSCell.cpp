#include "TBSCell.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"

// Costruttore dell'attore
ATBSCell::ATBSCell()
{
	// Non serve il Tick per una cella statica
	PrimaryActorTick.bCanEverTick = false;

	// Creiamo il componente mesh che rappresenta la cella
	CellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CellMesh"));

	// Impostiamo la mesh come RootComponent dell'attore
	RootComponent = CellMesh;

	// Carichiamo il cubo base di Unreal
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Se il cubo è stato trovato correttamente, lo assegniamo
	if (CubeMeshAsset.Succeeded())
	{
		CellMesh->SetStaticMesh(CubeMeshAsset.Object);
	}

	// Rendiamo il cubo una tile bassa
	SetActorScale3D(FVector(1.0f, 1.0f, 0.1f));

	// Valori iniziali della cella
	GridX = 0;
	GridY = 0;

	// Altezza base
	HeightLevel = 1;

	// Di default la cella è attraversabile
	bIsWalkable = true;

	// Stato iniziale di selezione
	bIsSelected = false;

	// Materiali inizialmente non assegnati
	DefaultMaterial = nullptr;
	SelectedMaterial = nullptr;
}

// Funzione chiamata quando la cella entra nel mondo
void ATBSCell::BeginPlay()
{
	Super::BeginPlay();

	// Applico il materiale normale all'inizio
	if (CellMesh && DefaultMaterial)
	{
		CellMesh->SetMaterial(0, DefaultMaterial);
	}
}

void ATBSCell::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;

	if (!CellMesh)
	{
		return;
	}

	// Se la cella è selezionata, applico il materiale di selezione
	if (bIsSelected && SelectedMaterial)
	{
		CellMesh->SetMaterial(0, SelectedMaterial);
	}
	// Altrimenti rimetto il materiale normale
	else if (DefaultMaterial)
	{
		CellMesh->SetMaterial(0, DefaultMaterial);
	}
}