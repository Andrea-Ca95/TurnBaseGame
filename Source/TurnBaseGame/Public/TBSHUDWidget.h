#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TBSHUDWidget.generated.h"

class UTextBlock;
class ATBSPlayerController;

UCLASS()
class TURNBASEGAME_API UTBSHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Chiamata quando il widget viene costruito
	virtual void NativeConstruct() override;

public:
	// Aggiorna tutti i testi del widget leggendo i dati dal PlayerController
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RefreshHUD();

	// Testo del turno corrente
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TurnText;

	// Testo delle torri del player umano
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HumanTowersText;

	// Testo delle torri della AI
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AITowersText;

	// Testo di stato delle unità umane
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HumanUnitsText;

	// Testo di stato delle unità AI
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AIUnitsText;
};