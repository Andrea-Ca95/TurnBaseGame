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

	// Carico una sfera base di Unreal come mesh di default
	// Le classi figlie potranno sostituirla
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Se la mesh è stata trovata correttamente, la assegno
	if (SphereMeshAsset.Succeeded())
	{
		UnitMesh->SetStaticMesh(SphereMeshAsset.Object);
	}

	// Scale base dell'unità
	NormalScale = FVector(0.4f, 0.4f, 0.4f);
	SelectedScale = FVector(0.5f, 0.5f, 0.5f);

	// Applico la scala normale iniziale
	SetActorScale3D(NormalScale);

	// Coordinate iniziali logiche
	GridX = 0;
	GridY = 0;

	// Range base di default
	MovementRange = 1;

	// All'inizio l'unità non è selezionata
	bIsSelected = false;
}

// Funzione chiamata quando l'unità entra nel mondo
void ATBSUnit::BeginPlay()
{
	Super::BeginPlay();
}

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

// Applica o rimuove la selezione visiva dell'unità
void ATBSUnit::SetSelected(bool bSelected)
{
	// Salvo lo stato interno
	bIsSelected = bSelected;

	// Se l'unità è selezionata, aumento leggermente la scala
	if (bIsSelected)
	{
		SetActorScale3D(SelectedScale);
	}
	else
	{
		// Altrimenti torno alla scala normale
		SetActorScale3D(NormalScale);
	}
}

// Restituisce il range massimo di movimento
int32 ATBSUnit::GetMovementRange() const
{
	return MovementRange;
}