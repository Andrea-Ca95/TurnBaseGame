#include "TBSBrawler.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Costruttore del Brawler
ATBSBrawler::ATBSBrawler()
{
	// Carico il cubo base di Unreal per distinguere visivamente il Brawler
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Se la mesh è stata trovata correttamente e la mesh dell'unità esiste, la assegno
	if (CubeMeshAsset.Succeeded() && UnitMesh)
	{
		UnitMesh->SetStaticMesh(CubeMeshAsset.Object);
	}

	// Imposto le scale specifiche del Brawler
	NormalScale = FVector(0.35f, 0.35f, 0.35f);
	SelectedScale = FVector(0.52f, 0.52f, 0.52f);
	SetActorScale3D(NormalScale);
	
	// Statistiche da specifica
	AttackRange = 1;
	MovementRange = 6;
	MinDamage = 1;
	MaxDamage = 6;
	MaxHealth = 40;
	CurrentHealth = MaxHealth;
}