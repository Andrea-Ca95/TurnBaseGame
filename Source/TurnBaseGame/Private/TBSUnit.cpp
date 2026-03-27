#include "TBSUnit.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Costruttore dell'unità
ATBSUnit::ATBSUnit()
{
	// Attivo il Tick perché ci serve per muovere l'unità lungo un percorso
	PrimaryActorTick.bCanEverTick = true;

	// Creo il componente mesh che rappresenta visivamente l'unità
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));

	// Imposto la mesh come RootComponent dell'attore
	RootComponent = UnitMesh;

	// Carico una sfera base di Unreal come mesh di default
	// Le classi figlie, come Sniper e Brawler, potranno sostituirla
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Se la mesh è stata trovata correttamente, la assegno all'unità
	if (SphereMeshAsset.Succeeded())
	{
		UnitMesh->SetStaticMesh(SphereMeshAsset.Object);
	}

	// Scala normale dell'unità quando non è selezionata
	NormalScale = FVector(0.4f, 0.4f, 0.4f);

	// Scala dell'unità quando è selezionata
	SelectedScale = FVector(0.5f, 0.5f, 0.5f);

	// Applico subito la scala normale iniziale
	SetActorScale3D(NormalScale);

	// Coordinate logiche iniziali dell'unità sulla griglia
	GridX = 0;
	GridY = 0;

	// Range di movimento base di default
	// Le classi figlie potranno sovrascriverlo
	MovementRange = 1;

	// Valori di combattimento base di default
	// Anche questi verranno sovrascritti da Sniper e Brawler
	AttackRange = 1;
	MinDamage = 1;
	MaxDamage = 1;
	MaxHealth = 10;

	// All'inizio la vita attuale coincide con la vita massima
	CurrentHealth = MaxHealth;

	// All'inizio del turno l'unità non ha ancora agito
	bHasMovedThisTurn = false;
	bHasAttackedThisTurn = false;

	// All'inizio del turno l'unità non ha ancora effettuato azioni
	bHasMovedThisTurn = false;
	bHasAttackedThisTurn = false;

	// All'inizio l'unità non è selezionata
	bIsSelected = false;

	// Il percorso parte dal primo punto della lista
	CurrentPathIndex = 0;

	// Velocità di movimento dell'unità lungo il percorso
	MoveSpeed = 450.0f;

	// All'inizio l'unità non si sta muovendo
	bIsMoving = false;

	OriginalGridX = 0;
	OriginalGridY = 0;
}

// Funzione chiamata quando l'unità entra nel mondo di gioco
void ATBSUnit::BeginPlay()
{
	Super::BeginPlay();
}

// Tick chiamato ogni frame
void ATBSUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Se l'unità non si sta muovendo, oppure non ha un percorso valido,
	// oppure ha già raggiunto la fine del percorso, esco subito
	if (!bIsMoving || MovementPath.Num() == 0 || CurrentPathIndex >= MovementPath.Num())
	{
		return;
	}

	// Prendo il punto corrente del percorso verso cui l'unità deve muoversi
	FVector TargetLocation = MovementPath[CurrentPathIndex];

	// Recupero la posizione attuale dell'unità nel mondo
	FVector CurrentLocation = GetActorLocation();

	// Calcolo la nuova posizione spostandomi verso il target a velocità costante
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);

	// Applico la nuova posizione all'unità
	SetActorLocation(NewLocation);

	// Se l'unità è arrivata abbastanza vicina al punto corrente del percorso
	if (FVector::Dist(NewLocation, TargetLocation) < 2.0f)
	{
		// La posiziono esattamente sul target
		SetActorLocation(TargetLocation);

		// Passo al punto successivo del percorso
		CurrentPathIndex++;

		// Se ho finito tutti i punti del percorso
		if (CurrentPathIndex >= MovementPath.Num())
		{
			// Segno che il movimento è terminato
			bIsMoving = false;

			// Svuoto il percorso
			MovementPath.Empty();

			// Resetto l'indice del percorso
			CurrentPathIndex = 0;
		}
	}
}

// Sposta immediatamente l'unità su una nuova cella
void ATBSUnit::MoveToCell(int32 NewGridX, int32 NewGridY, const FVector& NewWorldLocation)
{
	// Aggiorno la posizione logica dell'unità sulla griglia
	GridX = NewGridX;
	GridY = NewGridY;

	// Aggiorno la posizione visiva dell'unità nel mondo
	SetActorLocation(NewWorldLocation);

	// Scrivo nel log la nuova posizione per debug
	UE_LOG(LogTemp, Warning, TEXT("Unita spostata -> X: %d | Y: %d"), GridX, GridY);
}

// Avvia il movimento lungo un percorso composto da più punti
void ATBSUnit::StartPathMovement(int32 NewGridX, int32 NewGridY, const TArray<FVector>& PathPoints)
{
	// Aggiorno subito la posizione logica finale dell'unità
	GridX = NewGridX;
	GridY = NewGridY;

	// Salvo il percorso completo da seguire
	MovementPath = PathPoints;

	// Il movimento parte dal primo punto del percorso
	CurrentPathIndex = 0;

	// Se il percorso contiene almeno un punto, attivo il movimento
	bIsMoving = (MovementPath.Num() > 0);

	// Scrivo nel log la destinazione finale del percorso
	UE_LOG(LogTemp, Warning, TEXT("Movimento percorso avviato -> X: %d | Y: %d"), GridX, GridY);
}

// Applica o rimuove la selezione visiva dell'unità
void ATBSUnit::SetSelected(bool bSelected)
{
	// Salvo lo stato di selezione
	bIsSelected = bSelected;

	// Se l'unità è selezionata, applico la scala più grande
	if (bIsSelected)
	{
		SetActorScale3D(SelectedScale);
	}
	else
	{
		// Altrimenti torno alla scala normale
		SetActorScale3D(NormalScale);
	}
}

// Restituisce il range massimo di movimento dell'unità
int32 ATBSUnit::GetMovementRange() const
{
	return MovementRange;
}

// Restituisce il range massimo di attacco dell'unità
int32 ATBSUnit::GetAttackRange() const
{
	return AttackRange;
}

// Genera un danno casuale compreso tra MinDamage e MaxDamage
int32 ATBSUnit::RollDamage() const
{
	return FMath::RandRange(MinDamage, MaxDamage);
}

// Applica una quantità di danno all'unità
void ATBSUnit::ReceiveDamage(int32 DamageAmount)
{
	// Sottraggo il danno ricevuto alla vita attuale
	CurrentHealth -= DamageAmount;

	// Mantengo la vita nel range valido [0, MaxHealth]
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);

	// Scrivo nel log la vita rimanente
	UE_LOG(LogTemp, Warning, TEXT("Unita colpita: HP rimanenti = %d"), CurrentHealth);

	// Se la vita è arrivata a zero, l'unità viene eliminata
	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unita eliminata."));
	}
}

// Controlla se l'unità è morta
bool ATBSUnit::IsDead() const
{
	return CurrentHealth <= 0;
}

// Controlla se l'unità si sta muovendo lungo un percorso
bool ATBSUnit::IsMoving() const
{
	return bIsMoving;
}

// Segna che l'unità ha effettuato il movimento nel turno corrente
void ATBSUnit::MarkMoved()
{
	bHasMovedThisTurn = true;
}

// Segna che l'unità ha effettuato l'attacco nel turno corrente
void ATBSUnit::MarkAttacked()
{
	bHasAttackedThisTurn = true;
}

// Resetta lo stato del turno dell'unità
void ATBSUnit::ResetTurnState()
{
	bHasMovedThisTurn = false;
	bHasAttackedThisTurn = false;
}

// Controlla se l'unità può ancora muoversi
bool ATBSUnit::CanMoveThisTurn() const
{
	return !bHasMovedThisTurn;
}

// Controlla se l'unità può ancora attaccare
bool ATBSUnit::CanAttackThisTurn() const
{
	return !bHasAttackedThisTurn;
}

// Controlla se l'unità ha finito il turno
bool ATBSUnit::HasFinishedTurn() const
{
	// Il turno dell'unita è concluso solo se:
	// - ha attaccato (sia che abbia attaccato subito, sia dopo il movimento)
	// - oppure ha già mosso e non può più fare altre azioni nel flusso che gestiremo dopo
	//
	// In questa fase consideriamo concluso il turno solo dopo l'attacco.
	// Dopo il movimento l'unita resta selezionabile per poter ancora attaccare.
	return bHasAttackedThisTurn;
}

// Restituisce gli HP correnti dell'unita
int32 ATBSUnit::GetCurrentHealth() const
{
	return CurrentHealth;
}

// Restituisce gli HP massimi dell'unita
int32 ATBSUnit::GetMaxHealth() const
{
	return MaxHealth;
}

// Restituisce la posizione di spawn originaria dell'unità
void ATBSUnit::SetOriginalSpawnPosition(int32 InOriginalGridX, int32 InOriginalGridY)
{
	// Salvo la posizione originaria dell'unità per il futuro respawn
	OriginalGridX = InOriginalGridX;
	OriginalGridY = InOriginalGridY;
}

// Rignera completamente l'unità e la riporta alla cella di spawn originaria
void ATBSUnit::RespawnToOriginalCell(const FVector& RespawnWorldLocation)
{
	// Ripristino completamente gli HP
	CurrentHealth = MaxHealth;

	// Resetto lo stato del turno
	ResetTurnState();

	// L'unità non deve risultare selezionata
	SetSelected(false);

	// Interrompo eventuale movimento residuo
	bIsMoving = false;
	MovementPath.Empty();
	CurrentPathIndex = 0;

	// Riporto le coordinate logiche alla cella originaria
	GridX = OriginalGridX;
	GridY = OriginalGridY;

	// Riporto fisicamente l'unità nella posizione originaria
	SetActorLocation(RespawnWorldLocation);

	UE_LOG(LogTemp, Warning, TEXT("Unita respawnata in posizione originaria -> X: %d | Y: %d | HP: %d"),
		GridX, GridY, CurrentHealth);
}