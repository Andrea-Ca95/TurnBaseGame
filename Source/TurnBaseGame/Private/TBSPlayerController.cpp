#include "TBSPlayerController.h"
#include "TBSGridManager.h"
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
			// Cerco il GridManager presente nel livello
			ATBSGridManager* GridManager = nullptr;

			for (TActorIterator<ATBSGridManager> It(GetWorld()); It; ++It)
			{
				GridManager = *It;
				break;
			}

			// Se non trovo il GridManager, blocco la selezione per sicurezza
			if (!GridManager)
			{
				UE_LOG(LogTemp, Warning, TEXT("GridManager non trovato."));
				return;
			}

			// Controllo se l'unità cliccata appartiene al player umano
			if (GridManager->HumanUnits.Contains(ClickedUnit))
			{
				// Se era già selezionata un'altra unità, tolgo la selezione visiva
				if (CurrentlySelectedUnit && CurrentlySelectedUnit != ClickedUnit)
				{
					CurrentlySelectedUnit->SetSelected(false);
				}

				// Salvo la nuova unità come unità attualmente selezionata
				CurrentlySelectedUnit = ClickedUnit;

				// Applico la selezione visiva alla nuova unità
				CurrentlySelectedUnit->SetSelected(true);

				// Mostro il range di movimento dell'unità selezionata
				ShowMovementRange(CurrentlySelectedUnit);

				// Scrivo nel log la posizione logica dell'unità selezionata
				UE_LOG(LogTemp, Warning, TEXT("Unita umana selezionata -> X: %d | Y: %d"), ClickedUnit->GridX, ClickedUnit->GridY);
			}
			else
			{
				// Se l'unità non è del player umano, non la seleziono
				UE_LOG(LogTemp, Warning, TEXT("Questa unita non e controllabile dal player."));
			}

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

				// Tolgo la selezione visiva dall'unità dopo il movimento
				CurrentlySelectedUnit->SetSelected(false);

				// Nascondo il range di movimento dopo aver effettuato l'azione
				HideMovementRange();

				// Dopo il movimento, deseleziono l'unità
				CurrentlySelectedUnit = nullptr;

				// Scrivo nel log che il movimento è terminato
				UE_LOG(LogTemp, Warning, TEXT("Movimento completato: unita deselezionata."));
			}
		}
	}
}

// Nasconde il range di movimento attualmente mostrato
void ATBSPlayerController::HideMovementRange()
{
	for (ATBSCell* Cell : HighlightedMovementCells)
	{
		if (IsValid(Cell))
		{
			Cell->SetSelected(false);
		}
	}

	HighlightedMovementCells.Empty();
}

// Mostra il range di movimento dell'unità selezionata
void ATBSPlayerController::ShowMovementRange(ATBSUnit* Unit)
{
	// Se l'unità non è valida, esco
	if (!Unit)
	{
		return;
	}

	// Prima pulisco eventuale range precedente
	HideMovementRange();

	// Cerco il GridManager presente nel livello
	ATBSGridManager* GridManager = nullptr;

	for (TActorIterator<ATBSGridManager> It(GetWorld()); It; ++It)
	{
		GridManager = *It;
		break;
	}

	if (!GridManager)
	{
		return;
	}

	// Range massimo di movimento dell'unità
	int32 MaxRange = Unit->GetMovementRange();

	// Evidenzio tutte le celle raggiungibili in distanza Manhattan
	for (ATBSCell* Cell : GridManager->SpawnedCells)
	{
		if (!IsValid(Cell))
		{
			continue;
		}

		// Salto celle non attraversabili
		if (!Cell->bIsWalkable)
		{
			continue;
		}

		int32 Distance = FMath::Abs(Cell->GridX - Unit->GridX) + FMath::Abs(Cell->GridY - Unit->GridY);

		if (Distance > 0 && Distance <= MaxRange)
		{
			Cell->SetSelected(true);
			HighlightedMovementCells.Add(Cell);
		}
	}
}