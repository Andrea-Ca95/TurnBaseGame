#include "TBSUnit.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Costruttore dell'unità
ATBSUnit::ATBSUnit()
{
	// Non serve il Tick per ora
	PrimaryActorTick.bCanEverTick = false;

	// Creo la mesh dell'unità
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));

	// La mesh diventa la root dell'attore
	RootComponent = UnitMesh;

	// Carico una sfera base di Unreal per rappresentare l'unità
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Se la mesh è stata trovata correttamente, la assegno
	if (SphereMeshAsset.Succeeded())
	{
		UnitMesh->SetStaticMesh(SphereMeshAsset.Object);
	}

	// Riduco la dimensione dell'unità per farla stare bene sopra una cella
	SetActorScale3D(FVector(0.4f, 0.4f, 0.4f));

	// Coordinate iniziali logiche
	GridX = 0;
	GridY = 0;
}

// Funzione chiamata quando l'unità entra nel mondo
void ATBSUnit::BeginPlay()
{
	Super::BeginPlay();
}

// Funzione che sposta l'unità in una nuova cella
// Funzione che sposta l'unità in una nuova cella
void ATBSUnit::MoveToCell(int32 NewGridX, int32 NewGridY, const FVector& NewWorldLocation)
{
	// Aggiorno le coordinate logiche
	GridX = NewGridX;
	GridY = NewGridY;

	// Sposto realmente l'attore nel mondo
	SetActorLocation(NewWorldLocation);

	// Scrivo nel log la nuova posizione
	UE_LOG(LogTemp, Warning, TEXT("Unita spostata -> X: %d | Y: %d"), GridX, GridY);
}