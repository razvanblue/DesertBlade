// Fill out your copyright notice in the Description page of Project Settings.


#include "DesertBladeCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapons/Weapon.h"

ADesertBladeCharacter::ADesertBladeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 220.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
}

void ADesertBladeCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

void ADesertBladeCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::Unoccupied)
	{
		return;
	}
	
	if (Controller && Value != 0.f)
	{
	    const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation{ 0.f, ControlRotation.Yaw, 0.f };
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ADesertBladeCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::Unoccupied)
	{
		return;
	}
	
	if (Controller && Value != 0.f)
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation{ 0.f, ControlRotation.Yaw, 0.f };
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ADesertBladeCharacter::Turn(float Value)
{
}

void ADesertBladeCharacter::LookUp(float Value)
{
}

void ADesertBladeCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::Unoccupied)
	{
		return;
	}

	const auto MoveValue = Value.Get<FVector2D>();
	if (MoveValue != FVector2D::ZeroVector)
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation{ 0.f, ControlRotation.Yaw, 0.f };
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		AddMovementInput(ForwardDirection, MoveValue.Y);
		AddMovementInput(RightDirection, MoveValue.X);
		AnimInstance->Montage_Stop(0.3, AttackMontage);
	}
}

void ADesertBladeCharacter::Look(const FInputActionValue& Value)
{
	const auto LookValue = Value.Get<FVector2D>();
	if (LookValue != FVector2D::ZeroVector)
	{
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(LookValue.Y);
	}
}

void ADesertBladeCharacter::Interact()
{
	if (auto* Weapon = Cast<AWeapon>(OverlappingItem))
	{
		Weapon->Equip(GetMesh(), TEXT("hand_r_socket"), this, this);
		CharacterState = ECharacterState::Equipped1HWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = Weapon;
	}
	else if (CanDisarm())
	{
		PlayEquipMontage("Unequip");
		CharacterState = ECharacterState::Unequipped;
		ActionState = EActionState::Equipping;
	}
	else if (CanArm())
	{
		PlayEquipMontage("Equip");
		CharacterState = ECharacterState::Equipped1HWeapon;
		ActionState = EActionState::Equipping;
	}
}

void ADesertBladeCharacter::Attack()
{
	if (CharacterState == ECharacterState::Equipped1HWeapon && ActionState == EActionState::Unoccupied)
	{
		ActionState = EActionState::Attacking;
		PlayAttackMontage();
	}
}

void ADesertBladeCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		
		FName SectionName{};
		int32 AttackIdx = FMath::RandRange(0, 2);
		switch (AttackIdx)
		{
		case 0:
			SectionName = FName(TEXT("Attack1"));
			break;
		case 1:
			SectionName = FName(TEXT("Attack2"));
			break;
		case 2:
			SectionName = FName(TEXT("Attack3"));
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ADesertBladeCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ADesertBladeCharacter::AttackEnd()
{
	ActionState = EActionState::Unoccupied;
}

bool ADesertBladeCharacter::CanDisarm()
{
	return ActionState == EActionState::Unoccupied && CharacterState != ECharacterState::Unequipped;
}

bool ADesertBladeCharacter::CanArm()
{
	return ActionState == EActionState::Unoccupied
		&& CharacterState == ECharacterState::Unequipped
		&& EquippedWeapon;
}

void ADesertBladeCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), TEXT("hand_r_socket"));
	}
}

void ADesertBladeCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), TEXT("back_socket"));
	}
}

void ADesertBladeCharacter::SetHitboxCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetHitbox())
	{
		EquippedWeapon->IgnoreActors.Empty();
		EquippedWeapon->GetHitbox()->SetCollisionEnabled((CollisionEnabled));
	}
}

void ADesertBladeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADesertBladeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("MoveForward", this, &ADesertBladeCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &ADesertBladeCharacter::MoveRight);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADesertBladeCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADesertBladeCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ADesertBladeCharacter::Interact);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ADesertBladeCharacter::Attack);
	}
}

