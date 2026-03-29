#pragma once

// Include base di Unreal
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBSCell.h"
#include "TBSUnit.h"
#include "TBSTower.h"
#include "TBSGridManager.generated.h"

// Forward declaration per i materiali
class UMaterialInterface;
class ATBSSniper;
class ATBSBrawler;

UENUM()
enum class ETBSUnitType : uint8
{
	// Unità a distanza
	Sniper,

	// Unità da mischia
	Brawler
};

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

	// Chiamata quando il GridManager esce dal mondo o il gioco viene chiuso
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Crea le 3 torri sulla mappa dopo la generazione della griglia
	void SpawnTowers();

	// Cerca la cella valida più vicina a una coordinata ideale
	ATBSCell* FindBestTowerCell(int32 IdealX, int32 IdealY) const;

	// Crea le unità iniziali nelle zone di schieramento
	void SpawnInitialUnits();

	// Cerca una cella valida casuale nella zona del player umano
	ATBSCell* FindRandomValidHumanSpawnCell(const TArray<ATBSCell*>& ReservedCells) const;

	// Cerca una cella valida casuale nella zona AI
	ATBSCell* FindRandomValidAISpawnCell(const TArray<ATBSCell*>& ReservedCells) const;

	// Controlla se un bersaglio nemico è nel range di attacco dell'unità selezionata
	bool IsEnemyInAttackRange(class ATBSUnit* Attacker, class ATBSUnit* Target) const;

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

	// Materiale quando la cella è selezionata
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* SelectedCellMaterial;

	// Materiale delle celle con bersagli attaccabili
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInterface* AttackRangeCellMaterial;

	// Materiale delle unità del player umano
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Materials")
	UMaterialInterface* HumanUnitMaterial;

	// Materiale delle unità AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Materials")
	UMaterialInterface* AIUnitMaterial;

	// Materiale del livello 0 (acqua)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Materials")
	UMaterialInterface* Level0Material;

	// Materiale del livello 1
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Materials")
	UMaterialInterface* Level1Material;

	// Materiale del livello 2
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Materials")
	UMaterialInterface* Level2Material;

	// Materiale del livello 3
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Materials")
	UMaterialInterface* Level3Material;

	// Materiale del livello 4
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain Materials")
	UMaterialInterface* Level4Material;

	// Classe base unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	TSubclassOf<ATBSUnit> UnitClass;

	// Classe Sniper
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	TSubclassOf<ATBSSniper> SniperClass;

	// Classe Brawler
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	TSubclassOf<ATBSBrawler> BrawlerClass;

	// Classe della torre da spawnare sulla griglia
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower")
	TSubclassOf<ATBSTower> TowerClass;

	// Materiale torre neutrale
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower Materials")
	UMaterialInterface* TowerNeutralMaterial;

	// Materiale torre contesa
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower Materials")
	UMaterialInterface* TowerContestedMaterial;

	// Materiale torre controllata dal player umano
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower Materials")
	UMaterialInterface* TowerHumanControlledMaterial;

	// Materiale torre controllata dalla AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tower Materials")
	UMaterialInterface* TowerAIControlledMaterial;

	// Seed casuale usato per generare la mappa
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Generation")
	int32 MapSeed;

	// Scala del Perlin Noise
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Generation")
	float NoiseScale;

	// Numero massimo di livelli di altezza
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Generation")
	int32 MaxHeightLevel;

	// Array che contiene tutte le celle create nella griglia
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	TArray<ATBSCell*> SpawnedCells;

	// Array che contiene tutte le torri create nella mappa
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	TArray<ATBSTower*> SpawnedTowers;

	// Unità umane spawnate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	TArray<ATBSUnit*> HumanUnits;

	// Unità AI spawnate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	TArray<ATBSUnit*> AIUnits;

	// Funzione che genera tutta la griglia
	UFUNCTION(CallInEditor, Category = "Grid")
	void GenerateGrid();

	// Rimuove dagli array le unità non più valide o distrutte
	void CleanupDestroyedUnits();

	// Aggiorna lo stato di controllo di tutte le torri
	void UpdateTowerControlStates();

	// Conta quante unità umane sono nella zona di cattura di una torre
	int32 CountHumanUnitsInTowerZone(class ATBSTower* Tower) const;

	// Conta quante unità AI sono nella zona di cattura di una torre
	int32 CountAIUnitsInTowerZone(class ATBSTower* Tower) const;

	// Restituisce la cella alle coordinate richieste
	ATBSCell* GetCellAtCoordinates(int32 X, int32 Y) const;

	// Gestisce il respawn di una unità eliminata nella sua posizione originaria
	void HandleUnitRespawn(ATBSUnit* Unit);

	// Crea una singola unità del tipo richiesto nella cella indicata per il player specificato
	ATBSUnit* SpawnUnitAtCell(ETBSPlayerOwner PlayerOwner, ETBSUnitType UnitType, ATBSCell* TargetCell);

	// Controlla se una cella è già occupata da una unità umana o AI
	bool IsCellOccupiedByAnyUnit(int32 X, int32 Y) const;
};