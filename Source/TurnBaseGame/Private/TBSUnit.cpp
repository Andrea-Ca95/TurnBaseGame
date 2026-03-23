#include "TBSUnit.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Costruttore dell'unità
ATBSUnit::ATBSUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	RootComponent = UnitMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		UnitMesh->SetStaticMesh(SphereMeshAsset.Object);
	}

	NormalScale = FVector(0.4f, 0.4f, 0.4f);
	SelectedScale = FVector(0.5f, 0.5f, 0.5f);
	SetActorScale3D(NormalScale);

	GridX = 0;
	GridY = 0;

	MovementRange = 1;

	// Valori combat di default
	AttackRange = 1;
	MinDamage = 1;
	MaxDamage = 1;
	MaxHealth = 10;
	CurrentHealth = MaxHealth;

	bIsSelected = false;

	CurrentPathIndex = 0;
	MoveSpeed = 450.0f;
	bIsMoving = false;
}

void ATBSUnit::BeginPlay()
{
	Super::BeginPlay();
}

void ATBSUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMoving || MovementPath.Num() == 0 || CurrentPathIndex >= MovementPath.Num())
	{
		return;
	}

	FVector TargetLocation = MovementPath[CurrentPathIndex];
	FVector CurrentLocation = GetActorLocation();

	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);
	SetActorLocation(NewLocation);

	if (FVector::Dist(NewLocation, TargetLocation) < 2.0f)
	{
		SetActorLocation(TargetLocation);
		CurrentPathIndex++;

		if (CurrentPathIndex >= MovementPath.Num())
		{
			bIsMoving = false;
			MovementPath.Empty();
			CurrentPathIndex = 0;
		}
	}
}

void ATBSUnit::MoveToCell(int32 NewGridX, int32 NewGridY, const FVector& NewWorldLocation)
{
	GridX = NewGridX;
	GridY = NewGridY;
	SetActorLocation(NewWorldLocation);

	UE_LOG(LogTemp, Warning, TEXT("Unita spostata -> X: %d | Y: %d"), GridX, GridY);
}

void ATBSUnit::StartPathMovement(int32 NewGridX, int32 NewGridY, const TArray<FVector>& PathPoints)
{
	GridX = NewGridX;
	GridY = NewGridY;

	MovementPath = PathPoints;
	CurrentPathIndex = 0;
	bIsMoving = (MovementPath.Num() > 0);

	UE_LOG(LogTemp, Warning, TEXT("Movimento percorso avviato -> X: %d | Y: %d"), GridX, GridY);
}

void ATBSUnit::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;

	if (bIsSelected)
	{
		SetActorScale3D(SelectedScale);
	}
	else
	{
		SetActorScale3D(NormalScale);
	}
}

int32 ATBSUnit::GetMovementRange() const
{
	return MovementRange;
}

int32 ATBSUnit::GetAttackRange() const
{
	return AttackRange;
}

int32 ATBSUnit::RollDamage() const
{
	return FMath::RandRange(MinDamage, MaxDamage);
}

void ATBSUnit::ReceiveDamage(int32 DamageAmount)
{
	CurrentHealth -= DamageAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Unita colpita: HP rimanenti = %d"), CurrentHealth);

	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unita eliminata."));
		Destroy();
	}
}

bool ATBSUnit::IsDead() const
{
	return CurrentHealth <= 0;
}

bool ATBSUnit::IsMoving() const
{
	return bIsMoving;
}