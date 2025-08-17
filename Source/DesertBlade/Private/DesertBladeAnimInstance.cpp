// Fill out your copyright notice in the Description page of Project Settings.


#include "DesertBladeAnimInstance.h"
#include "DesertBladeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UDesertBladeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Character =	Cast<ADesertBladeCharacter>(TryGetPawnOwner());
	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
	}
}

void UDesertBladeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
		isFalling = CharacterMovement->IsFalling();
		CharacterState = Character->GetCharacterState();
	}
}
