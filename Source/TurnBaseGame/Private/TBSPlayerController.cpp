#include "TBSPlayerController.h"
#include "TBSCell.h"
#include "TBSUnit.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "EngineUtils.h"
#include "InputCoreTypes.h"

// Costruttore del PlayerController
ATBSPlayerController::ATBSPlayerController()
{
	// Attiva il Tick del PlayerController
	PrimaryActorTick.bCanEverTick = true;

	// Mostra il cursore del mouse
	bShowMouseCursor = true;

	// Abilita eventi di click e hover
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// All'inizio nessuna cella è selezionata
	CurrentlySelectedCell = nullptr;

	// All'inizio nessuna unità è selezionata
	CurrentlySelectedUnit = nullptr;
}

void ATBSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Imposto un input mode misto: il gioco continua a ricevere input,
	// ma posso anche usare il mouse nell'interfaccia
	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);

	// Cerco una CameraActor presente nel livello
	// e la imposto come camera principale del giocatore
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		// Uso la prima camera trovata nel livello
		SetViewTarget(*It);
		break;
	}
}

void ATBSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// Se il tasto sinistro del mouse viene premuto in questo frame,
	// gestisco il click manualmente
	if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		OnLeftMouseClick();
	}
}

void ATBSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ATBSPlayerController::OnLeftMouseClick()
{
	FHitResult HitResult;

	// Faccio un raycast sotto il cursore del mouse
	bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	// Se il raycast ha colpito qualcosa e l'attore esiste, continuo
	if (bHit && HitResult.GetActor())
	{
		// Prima controllo se ho cliccato un'unità
		ATBSUnit* ClickedUnit = Cast<ATBSUnit>(HitResult.GetActor());

		if (ClickedUnit)
		{
			// Salvo l'unità come unità attualmente selezionata
			CurrentlySelectedUnit = ClickedUnit;

			// Scrivo nel log la posizione logica dell'unità selezionata
			UE_LOG(LogTemp, Warning, TEXT("Unita selezionata -> X: %d | Y: %d"), ClickedUnit->GridX, ClickedUnit->GridY);
			return;
		}

		// Se non ho cliccato un'unità, provo a vedere se ho cliccato una cella
		ATBSCell* ClickedCell = Cast<ATBSCell>(HitResult.GetActor());

		if (ClickedCell)
		{
			// Se c'era una cella selezionata prima e non è la stessa,
			// tolgo la selezione visiva dalla vecchia cella
			if (CurrentlySelectedCell && CurrentlySelectedCell != ClickedCell)
			{
				CurrentlySelectedCell->SetSelected(false);
			}

			// Salvo la nuova cella come selezionata
			CurrentlySelectedCell = ClickedCell;

			// Applico il materiale di selezione alla nuova cella
			CurrentlySelectedCell->SetSelected(true);

			// Scrivo nel log le coordinate della cella cliccata
			UE_LOG(LogTemp, Warning, TEXT("Cella cliccata -> X: %d | Y: %d"), ClickedCell->GridX, ClickedCell->GridY);

			// Se ho un'unità selezionata, la sposto sulla cella cliccata
			// Se ho un'unità selezionata, la sposto sulla cella cliccata
			// Se ho un'unità selezionata, provo a spostarla sulla cella cliccata
			if (CurrentlySelectedUnit)
			{
				// Prima controllo se la cella è attraversabile
				if (!ClickedCell->bIsWalkable)
				{
					// Se non è attraversabile, blocco il movimento
					UE_LOG(LogTemp, Warning, TEXT("Movimento bloccato: cella non attraversabile."));
					return;
				}

				// Prendo la posizione della cella cliccata
				FVector TargetLocation = ClickedCell->GetActorLocation();

				// Alzo la posizione in Z per tenere la sfera sopra la tile
				TargetLocation.Z += 60.0f;

				// Sposto l'unità sia logicamente sia visivamente
				CurrentlySelectedUnit->MoveToCell(ClickedCell->GridX, ClickedCell->GridY, TargetLocation);

				// Dopo il movimento, deseleziono l'unità
				// così per spostarla di nuovo bisogna ricliccarla
				CurrentlySelectedUnit = nullptr;

				// Scrivo nel log che il movimento è terminato
				UE_LOG(LogTemp, Warning, TEXT("Movimento completato: unita deselezionata."));
			}
		}
	}
}