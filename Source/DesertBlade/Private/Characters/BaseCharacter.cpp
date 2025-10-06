// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"

#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage) const
{
	if (Montage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(Montage);
		
		const int32 SectionIndex = FMath::RandRange(0, Montage->CompositeSections.Num() - 1);
		const FName& SectionName = Montage->CompositeSections[SectionIndex].SectionName;
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

double ABaseCharacter::CalculateImpactAngle(const FVector& ImpactPoint) const
{
	const FVector ImpactXY{ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z};
	const FVector Forward = GetActorForwardVector();
	const FVector ForwardXY{Forward.X, Forward.Y, 0.f};
	const FVector HitDirection = (ImpactXY - GetActorLocation()).GetSafeNormal();

	// Forward * HitDirection = |Forward| * |HitDirection| * cos(a)
	// |Forward| = |HitDirection| = 1 => Forward * HitDirection = cos(a)
	const double CosAngle = FVector::DotProduct(ForwardXY, HitDirection);
	double Angle = FMath::RadiansToDegrees(FMath::Acos(CosAngle));

	const FVector CrossProduct = FVector::CrossProduct(ForwardXY, HitDirection);
	// if Left-handed cross product points down, Angle should be negative
	if (CrossProduct.Z < 0.f)
	{
		Angle *= -1.f;
	}

	return Angle;
}

void ABaseCharacter::DirectionalHitReact(double HitAngle)
{

	FName Section{"FromBack"};
	if (HitAngle >=-45.f && HitAngle < 45.f)
	{
		Section = "FromFront";
	}
	else if (HitAngle >= -135.f && HitAngle < -45.f)
	{
		Section = "FromLeft";
	}
	else if (HitAngle >= 45.f && HitAngle < 135.f)
	{
		Section = "FromRight";
	}
	PlayMontageSection(HitReactMontage, Section);
}

void ABaseCharacter::OnDeath()
{
}

void ABaseCharacter::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	
	Super::Destroyed();
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetHitboxCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetHitbox())
	{
		EquippedWeapon->IgnoreActors.Empty();
		EquippedWeapon->GetHitbox()->SetCollisionEnabled(CollisionEnabled);
	}
}