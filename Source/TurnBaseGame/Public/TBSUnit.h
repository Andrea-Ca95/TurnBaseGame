#pragma once

// Include base di Unreal
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBSUnit.generated.h"

// Forward declaration per evitare include inutili nell'header
class UStaticMeshComponent;

UCLASS()
class TURNBASEGAME_API ATBSUnit : public AActor
{
	GENERATED_BODY()

public:
	// Costruttore dell'unità
	ATBSUnit();

protected:
	// Funzione chiamata quando l'unità entra nel mondo
	virtual void BeginPlay() override;

	// Tick usato per muovere l'unità lungo il percorso
	virtual void Tick(float DeltaTime) override;

	// Scala normale dell'unità
	FVector NormalScale;

	// Scala quando l'unità è selezionata
	FVector SelectedScale;

	// Stato di selezione visiva
	bool bIsSelected;

	// Lista delle posizioni mondo da seguire
	TArray<FVector> MovementPath;

	// Indice del punto corrente del percorso
	int32 CurrentPathIndex;

	// Velocità di movimento dell'unità
	float MoveSpeed;

	// Indica se l'unità si sta muovendo lungo un percorso
	bool bIsMoving;

public:
	// Mesh visiva dell'unità
	UPROPERTY(VisibleAnywhere, Category = "Unit")
	UStaticMeshComponent* UnitMesh;

	// Coordinata X della cella occupata dall'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 GridX;

	// Coordinata Y della cella occupata dall'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 GridY;

	// Numero massimo di celle di movimento dell'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 MovementRange;

	// Range massimo di attacco
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 AttackRange;

	// Danno minimo inflitto
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MinDamage;

	// Danno massimo inflitto
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MaxDamage;

	// Vita massima dell'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MaxHealth;

	// Vita attuale dell'unità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 CurrentHealth;

	// Indica se l'unità ha già effettuato il movimento nel turno corrente
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn")
	bool bHasMovedThisTurn;

	// Indica se l'unità ha già effettuato l'attacco nel turno corrente
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn")
	bool bHasAttackedThisTurn;

	// Sposta l'unità direttamente su una cella
	void MoveToCell(int32 NewGridX, int32 NewGridY, const FVector& NewWorldLocation);

	// Avvia il movimento lungo un percorso cella per cella
	void StartPathMovement(int32 NewGridX, int32 NewGridY, const TArray<FVector>& PathPoints);

	// Applica o rimuove la selezione visiva dell'unità
	void SetSelected(bool bSelected);

	// Restituisce il range massimo di movimento
	int32 GetMovementRange() const;

	// Restituisce il range massimo di attacco
	int32 GetAttackRange() const;

	// Restituisce un danno random nel range previsto
	int32 RollDamage() const;

	// Applica danno all'unità
	void ReceiveDamage(int32 DamageAmount);

	// Controlla se l'unità è morta
	bool IsDead() const;

	// Indica se l'unità si sta muovendo
	bool IsMoving() const;

	// Segna che l'unità ha effettuato il movimento nel turno corrente
	void MarkMoved();

	// Segna che l'unità ha effettuato l'attacco nel turno corrente
	void MarkAttacked();

	// Resetta lo stato del turno dell'unità
	void ResetTurnState();

	// Controlla se l'unità può ancora muoversi
	bool CanMoveThisTurn() const;

	// Controlla se l'unità può ancora attaccare
	bool CanAttackThisTurn() const;

	// Controlla se l'unità ha finito il turno
	bool HasFinishedTurn() const;
};