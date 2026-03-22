#include "TBSTower.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Costruttore della torre
ATBSTower::ATBSTower()
{
	// La torre non ha bisogno di Tick
	PrimaryActorTick.bCanEverTick = false;

	// Creo la mesh della torre
	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));

	// Imposto la mesh come root dell'attore
	RootComponent = TowerMesh;

	// Carico il cilindro base di Unreal per rappresentare la torre
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	// Se la mesh è stata trovata correttamente, la assegno
	if (CylinderMeshAsset.Succeeded())
	{
		TowerMesh->SetStaticMesh(CylinderMeshAsset.Object);
	}

	// Rendo la torre ben visibile sopra la griglia
	SetActorScale3D(FVector(0.6f, 0.6f, 1.5f));

	// Coordinate iniziali logiche
	GridX = 0;
	GridY = 0;
}

// Chiamata quando la torre entra nel mondo
void ATBSTower::BeginPlay()
{
	Super::BeginPlay();
}