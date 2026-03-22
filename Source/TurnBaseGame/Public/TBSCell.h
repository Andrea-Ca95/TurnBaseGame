#pragma once

// Include base di Unreal
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBSCell.generated.h"

// Forward declarations per evitare include inutili nell'header
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

	// Stato interno di selezione della cella
	bool bIsSelected;

public:
	// Mesh visiva della cella
	UPROPERTY(VisibleAnywhere, Category = "Cell")
	UStaticMeshComponent* CellMesh;

	// Coordinata X della cella nella griglia
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cell")
	int32 GridX;

	// Coordinata Y della cella nella griglia
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cell")
	int32 GridY;

	// Livello di altezza della cella
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cell")
	int32 HeightLevel;

	// Indica se una unità può camminare su questa cella
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cell")
	bool bIsWalkable;

	// Materiale base del terreno della cella
	// Questo dipenderà dal livello di altezza
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* BaseTerrainMaterial;

	// Materiale quando la cella è selezionata
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* SelectedMaterial;

	// Applica o rimuove lo stato di selezione visiva della cella
	void SetSelected(bool bSelected);

	// Aggiorna aspetto e posizione della cella in base all'altezza
	void UpdateVisualFromHeight(float CellSize);
};