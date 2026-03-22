#include "TBSGameMode.h"
#include "TBSPlayerController.h"

ATBSGameMode::ATBSGameMode()
{
	// Imposta il nostro PlayerController personalizzato
	PlayerControllerClass = ATBSPlayerController::StaticClass();
}