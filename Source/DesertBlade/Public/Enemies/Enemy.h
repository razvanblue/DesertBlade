// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class UHealthBarComponent;

UCLASS()
class DESERTBLADE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemy();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	
	virtual void BeginPlay() override;

	/**
	 * Checks if Enemy is within given radius of taget
	 * @param Target Must not be null
	 * @param Radius Radius around Target to check
	 */
	bool InTargetRange(const AActor* Target, double Radius);
	
	void PatrolTimerFinished();

	/**
	 * Patrol towards given target actor
	 * @param Target Target to patrol towards, must be valid
	 */
	void MoveToTarget(const AActor* Target);

	AActor* ChoosePatrolTarget();

	void CheckCombatTarget();
	
	void CheckPatrolTarget();

	void DirectionalDeath(double HitAngle);

	virtual void Attack() override;
	
	virtual void OnDeath() override;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	
	// ***** Components *****
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensing;

	// ***** Params and Statuses *****

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;
	
	UPROPERTY(BlueprintReadOnly)
	EEnemyState State = EEnemyState::Patrolling;
	
	UPROPERTY()
	AActor* CombatTarget;
	
	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;
	
	// Navigation
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<const AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolDelayMin = 3.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolDelayMax = 6.f;

	FTimerHandle PatrolTimer;

	
	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;
};
