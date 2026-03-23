#pragma once

// Include base di Unreal
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TBSPlayerController.generated.h"

// Forward declarations per evitare include pesanti nell'header
class ATBSCell;
class ATBSUnit;

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
};