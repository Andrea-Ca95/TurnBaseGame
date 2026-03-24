#include "TBSSniper.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Costruttore dello Sniper
ATBSSniper::ATBSSniper()
{
	// Carico la mesh sfera base di Unreal per distinguere visivamente lo Sniper
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Se la mesh è stata trovata correttamente e la mesh dell'unità esiste, la assegno
	if (SphereMeshAsset.Succeeded() && UnitMesh)
	{
		UnitMesh->SetStaticMesh(SphereMeshAsset.Object);
	}

	// Imposto le scale specifiche dello Sniper
	NormalScale = FVector(0.45f, 0.45f, 0.45f);
	SelectedScale = FVector(0.65f, 0.65f, 0.65f);
	SetActorScale3D(NormalScale);

	// Statistiche da specifica
	AttackRange = 10;
	MovementRange = 4;
	MinDamage = 4;
	MaxDamage = 8;
	MaxHealth = 20;
	CurrentHealth = MaxHealth;
}