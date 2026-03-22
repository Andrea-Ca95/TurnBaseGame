#pragma once

// Include base di Unreal
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBSGridManager.generated.h"

// Forward declarations per evitare include inutili nell'header
class ATBSCell;
class UMaterialInterface;
class ATBSUnit;

UCLASS()
class TURNBASEGAME_API ATBSGridManager : public AActor
{
	GENERATED_BODY()

public:
	// Costruttore del GridManager
	ATBSGridManager();

protected:
	// Funzione chiamata quando l'attore entra nel mondo
	virtual void BeginPlay() override;

	// Funzione che crea l'unità iniziale solo durante il gioco
	void SpawnInitialUnit();

public:
	// Numero di celle sull'asse X della griglia
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridWidth;

	// Numero di celle sull'asse Y della griglia
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridHeight;

	// Distanza tra una cella e l'altra
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	float CellSize;

	// Classe della cella da spawnare
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	TSubclassOf<ATBSCell> CellClass;

	// Materiale normale della cella
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* DefaultCellMaterial;

	// Materiale quando la cella è selezionata
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* SelectedCellMaterial;

	// Classe dell'unità da spawnare sulla griglia
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	TSubclassOf<ATBSUnit> UnitClass;

	// Coordinata X iniziale dell'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 InitialUnitX;

	// Coordinata Y iniziale dell'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 InitialUnitY;

	// Array che contiene tutte le celle create nella griglia
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	TArray<ATBSCell*> SpawnedCells;

	// Riferimento all'unità attualmente spawnata dal GridManager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	ATBSUnit* SpawnedUnit;

	// Funzione che genera tutta la griglia
	UFUNCTION(CallInEditor, Category = "Grid")
	void GenerateGrid();
};