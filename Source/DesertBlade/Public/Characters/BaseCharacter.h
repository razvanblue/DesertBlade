// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
class UAttributeComponent;

UCLASS()
class DESERTBLADE_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	virtual void Attack();

	virtual void OnDeath();

	virtual void Destroyed() override;
	
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName) const;

	void PlayRandomMontageSection(UAnimMontage* Montage) const;
	
	double CalculateImpactAngle(const FVector& ImpactPoint) const;
	
	void DirectionalHitReact(double HitAngle);
	
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	
	UFUNCTION(BlueprintCallable)
	void SetHitboxCollision(ECollisionEnabled::Type CollisionEnabled);
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;
	
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = Sounds)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	TObjectPtr<UParticleSystem> HitParticleSystem;
	
	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactMontage;
};
