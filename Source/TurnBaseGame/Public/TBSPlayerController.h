#pragma once

// Include base di Unreal
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TBSPlayerController.generated.h"

// Forward declarations per evitare include pesanti nell'header
class ATBSCell;
class ATBSUnit;

UENUM()
enum class ETBSTurnState : uint8
{
	Human,
	AI
};

UCLASS()
class TURNBASEGAME_API ATBSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Costruttore del PlayerController
	ATBSPlayerController();

protected:
	// Chiamata quando il controller entra in gioco
	virtual void BeginPlay() override;

	// Chiamata quando il controller esce dal mondo o il gioco viene chiuso
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Tick del PlayerController
	virtual void PlayerTick(float DeltaTime) override;

	// Binding degli input
	virtual void SetupInputComponent() override;

	// Funzione chiamata al click del mouse
	void OnLeftMouseClick();

	// Cella attualmente selezionata
	ATBSCell* CurrentlySelectedCell;

	// Unità attualmente selezionata
	ATBSUnit* CurrentlySelectedUnit;

	// Unita attualmente impegnata nella propria azione di turno
	class ATBSUnit* LockedTurnUnit;

	// Celle attualmente evidenziate come range di movimento
	TArray<class ATBSCell*> HighlightedMovementCells;

	// Celle che contengono bersagli attaccabili
	TArray<class ATBSCell*> HighlightedAttackCells;

	// Mostra i bersagli attaccabili dell'unita selezionata
	void ShowAttackRange(class ATBSUnit* Unit);

	// Nasconde i bersagli attaccabili attualmente mostrati
	void HideAttackRange();

	// Mostra il range di movimento dell'unità selezionata
	void ShowMovementRange(ATBSUnit* Unit);

	// Nasconde il range di movimento attualmente mostrato
	void HideMovementRange();

	// Restituisce il GridManager presente nel livello
	class ATBSGridManager* GetGridManager() const;

	// Cerca una cella tramite coordinate di griglia
	class ATBSCell* GetCellAtCoordinates(int32 X, int32 Y) const;

	// Controlla se una cella è occupata da un'altra unità
	bool IsCellOccupied(int32 X, int32 Y, const class ATBSUnit* IgnoredUnit = nullptr) const;

	// Calcola tutte le celle raggiungibili da una unità in base al costo del terreno
	void ComputeReachableCells(class ATBSUnit* Unit, TArray<class ATBSCell*>& OutReachableCells) const;

	// Controlla se un bersaglio nemico è nel range di attacco dell'unità selezionata
	bool IsEnemyInAttackRange(class ATBSUnit* Attacker, class ATBSUnit* Target) const;
	
	// Calcola il percorso minimo tra l'unità e una cella destinazione
	bool FindPathToCell(class ATBSUnit* Unit, class ATBSCell* DestinationCell, TArray<class ATBSCell*>& OutPathCells) const;

	// Converte un percorso di celle in posizioni mondo
	void BuildWorldPathFromCells(const TArray<class ATBSCell*>& PathCells, TArray<FVector>& OutWorldPath) const;
	
	// Stato corrente del turno di gioco
	ETBSTurnState CurrentTurnState;

	// Avvia il turno umano
	void StartHumanTurn();

	// Avvia il turno AI
	void StartAITurn();

	// Controlla se tutte le unità umane hanno concluso il turno
	bool HaveAllHumanUnitsFinishedTurn() const;

	// Numero di turni consecutivi in cui Human controlla almeno 2 torri
	int32 HumanConsecutiveTowerControlTurns;

	// Numero di turni consecutivi in cui AI controlla almeno 2 torri
	int32 AIConsecutiveTowerControlTurns;

	// Indica se la partita è terminata
	bool bGameEnded;

	// Conta quante torri sono controllate dal player umano
	int32 CountHumanControlledTowers() const;

	// Conta quante torri sono controllate dalla AI
	int32 CountAIControlledTowers() const;

	// Aggiorna la condizione di vittoria in base allo stato delle torri
	void UpdateVictoryCondition();

	// Esegue il turno della AI
	void ExecuteAITurn();

	// Esegue l'azione di una singola unità AI
	void ExecuteSingleAIUnitTurn(class ATBSUnit* AIUnit);

	// Cerca il bersaglio umano attaccabile più vicino per una unità AI
	class ATBSUnit* FindBestHumanTargetForAI(class ATBSUnit* AIUnit) const;

	// Cerca la torre più vicina che non è controllata dalla AI
	class ATBSTower* FindBestTowerTargetForAI(class ATBSUnit* AIUnit) const;

	// Cerca la migliore cella raggiungibile per avvicinarsi a una torre
	class ATBSCell* FindBestReachableCellTowardTower(class ATBSUnit* AIUnit, class ATBSTower* TargetTower) const;

	// Controlla se il player umano non ha più unità vive
	bool HasHumanLostAllUnits() const;

	// Controlla se la AI non ha più unità vive
	bool HasAILostAllUnits() const;
};