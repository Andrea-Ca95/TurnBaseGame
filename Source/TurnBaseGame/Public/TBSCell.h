#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBSCell.generated.h"

// Forward declaration per evitare include inutili
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class TURNBASEGAME_API ATBSCell : public AActor
{
	GENERATED_BODY()

public:

	// Costruttore della cella
	ATBSCell();

protected:

	// Funzione chiamata quando l'attore entra nel mondo
	virtual void BeginPlay() override;

	

	// Stato di selezione della cella
	bool bIsSelected;

public:

	// Mesh visiva della cella (il cubo che vedremo nella griglia)
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CellMesh;

	// Coordinata X della cella nella griglia
	UPROPERTY(EditAnywhere)
	int32 GridX;

	// Coordinata Y della cella nella griglia
	UPROPERTY(EditAnywhere)
	int32 GridY;

	// Livello di altezza della cella (lo useremo per le colline)
	UPROPERTY(EditAnywhere)
	int32 HeightLevel;

	// Indica se l'unità può camminare su questa cella
	UPROPERTY(EditAnywhere)
	bool bIsWalkable;
	// Materiale normale della cella
	UPROPERTY(EditAnywhere, Category = "Visual")
	UMaterialInterface* DefaultMaterial;

	// Materiale quando la cella è selezionata
	UPROPERTY(EditAnywhere, Category = "Visual")
	UMaterialInterface* SelectedMaterial;

	// Imposta lo stato di selezione della cella
	void SetSelected(bool bSelected);
};