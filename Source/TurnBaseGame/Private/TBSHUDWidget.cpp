#include "TBSHUDWidget.h"
#include "Components/TextBlock.h"
#include "TBSPlayerController.h"

// Chiamata quando il widget viene costruito
void UTBSHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Aggiorno subito la UI appena il widget viene creato
	RefreshHUD();
}

// Aggiorna tutti i testi del widget leggendo i dati dal PlayerController
void UTBSHUDWidget::RefreshHUD()
{
	// Recupero il PlayerController proprietario del widget
	ATBSPlayerController* TBSPlayerController = Cast<ATBSPlayerController>(GetOwningPlayer());
	if (!TBSPlayerController)
	{
		return;
	}

	// Aggiorno il testo del turno corrente
	if (TurnText)
	{
		TurnText->SetText(FText::FromString(FString::Printf(
			TEXT("Turno: %s"),
			*TBSPlayerController->GetCurrentTurnText())));
	}

	// Aggiorno il numero di torri conquistate dal player umano
	if (HumanTowersText)
	{
		HumanTowersText->SetText(FText::FromString(FString::Printf(
			TEXT("Torri Human: %d"),
			TBSPlayerController->GetHumanControlledTowerCountForUI())));
	}

	// Aggiorno il numero di torri conquistate dalla AI
	if (AITowersText)
	{
		AITowersText->SetText(FText::FromString(FString::Printf(
			TEXT("Torri AI: %d"),
			TBSPlayerController->GetAIControlledTowerCountForUI())));
	}

	// Aggiorno il testo con lo stato delle unità umane
	if (HumanUnitsText)
	{
		HumanUnitsText->SetText(FText::FromString(
			TBSPlayerController->GetHumanUnitsStatusText()));
	}

	// Aggiorno il testo con lo stato delle unità AI
	if (AIUnitsText)
	{
		AIUnitsText->SetText(FText::FromString(
			TBSPlayerController->GetAIUnitsStatusText()));
	}
}