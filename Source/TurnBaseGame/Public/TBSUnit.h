#pragma once

// Include base di Unreal
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBSUnit.generated.h"

// Forward declaration per evitare include inutili nell'header
class UStaticMeshComponent;

UCLASS()
class TURNBASEGAME_API ATBSUnit : public AActor
{
	GENERATED_BODY()

public:
	// Costruttore dell'unità
	ATBSUnit();

protected:
	// Funzione chiamata quando l'unità entra nel mondo
	virtual void BeginPlay() override;

	// Scala normale dell'unità
	FVector NormalScale;

	// Scala quando l'unità è selezionata
	FVector SelectedScale;

	// Stato di selezione visiva
	bool bIsSelected;

public:
	// Mesh visiva dell'unità
	UPROPERTY(VisibleAnywhere, Category = "Unit")
	UStaticMeshComponent* UnitMesh;

	// Coordinata X della cella occupata dall'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 GridX;

	// Coordinata Y della cella occupata dall'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 GridY;

	// Numero massimo di celle di movimento dell'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 MovementRange;

	// Sposta l'unità su una nuova cella sia logicamente sia visivamente
	void MoveToCell(int32 NewGridX, int32 NewGridY, const FVector& NewWorldLocation);

	// Applica o rimuove la selezione visiva dell'unità
	void SetSelected(bool bSelected);

	// Restituisce il range massimo di movimento
	int32 GetMovementRange() const;
};