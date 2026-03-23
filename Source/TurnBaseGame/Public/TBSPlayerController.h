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

	// Mostra il range di movimento dell'unità selezionata
	void ShowMovementRange(ATBSUnit* Unit);

	// Nasconde il range di movimento attualmente mostrato
	void HideMovementRange();
};