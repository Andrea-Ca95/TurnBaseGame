#pragma once

// Include base di Unreal
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBSTower.generated.h"

// Forward declaration per evitare include inutili
class UStaticMeshComponent;

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

	// Coordinata X della torre sulla griglia
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower")
	int32 GridX;

	// Coordinata Y della torre sulla griglia
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower")
	int32 GridY;
};