#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBSTower.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;

// Identifica quale player controlla la torre
UENUM()
enum class ETBSPlayerOwner : uint8
{
	None,
	Human,
	AI
};

// Stato interno della torre
UENUM()
enum class ETBSTowerState : uint8
{
	Neutral,
	Controlled,
	Contested
};

UCLASS()
class TURNBASEGAME_API ATBSTower : public AActor
{
	GENERATED_BODY()

public:
	// Costruttore della torre
	ATBSTower();

protected:
	// Chiamata quando la torre entra nel mondo
	virtual void BeginPlay() override;

public:
	// Mesh visiva della torre
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	UStaticMeshComponent* TowerMesh;

	// Coordinata X logica della torre
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower")
	int32 GridX;

	// Coordinata Y logica della torre
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower")
	int32 GridY;

	// Stato corrente della torre
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	ETBSTowerState TowerState;

	// Player che controlla la torre
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	ETBSPlayerOwner TowerOwner;

	// Materiale torre neutrale
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* NeutralMaterial;

	// Materiale torre contesa
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* ContestedMaterial;

	// Materiale torre controllata dal player umano
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* HumanControlledMaterial;

	// Materiale torre controllata dalla AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* AIControlledMaterial;

	// Imposta la torre come neutrale
	void SetNeutral();

	// Imposta la torre come controllata da un player
	void SetControlled(ETBSPlayerOwner NewOwner);

	// Imposta la torre come contesa
	void SetContested();

	// Aggiorna il materiale visivo della torre in base allo stato corrente
	void UpdateTowerVisual();
};