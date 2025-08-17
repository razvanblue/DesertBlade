// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "DesertBladeCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;
class UAnimMontage;
class UInputAction;
class UInputMappingContext;
class AItem;

UCLASS()
class DESERTBLADE_API ADesertBladeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADesertBladeCharacter();

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	
protected:
	virtual void BeginPlay() override;

	/************ Legacy Input *************/
	void MoveForward(float Value);
	void MoveRight(float Value);

	void Turn(float Value);
	void LookUp(float Value);
	
	/************ Enhanced Input *************/
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Interact();
	
	void Attack();
	
	/**************************/

	/**
	 * Play Attack Montage
	 */
	void PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> AttackAction;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;

	/**
	 * Animation Montages
	 */
	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	ECharacterState CharacterState = ECharacterState::Unequipped;
	
	UPROPERTY(BlueprintReadWrite)
	EActionState ActionState = EActionState::Unoccupied;

};
