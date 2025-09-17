// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "DesertBlade/DebugMacros.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarComponent"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercentage());
	HealthBarWidget->SetVisibility(false);
}

void AEnemy::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget)
	{
		const auto DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Length();
		if (DistanceToTarget > CombatRadius)
		{
			CombatTarget = nullptr;

			if (HealthBarWidget)
			{
				HealthBarWidget->SetVisibility(false);				
			}
		}
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

double AEnemy::CalculateImpactAngle(const FVector& ImpactPoint) const
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

void AEnemy::DirectionalDeath(double HitAngle)
{
	FName Section{"DeathForward"};
	DeathPose = EDeathPose::DeadForward;
	if (HitAngle >=-45.f && HitAngle < 45.f)
	{
		Section = "DeathBack";
		DeathPose = EDeathPose::DeadBack;
	}
	else if (HitAngle >= -135.f && HitAngle < -45.f)
	{
		Section = "DeathRight";
		DeathPose = EDeathPose::DeadRight;
	}
	else if (HitAngle >= 45.f && HitAngle < 135.f)
	{
		Section = "DeathLeft";
		DeathPose = EDeathPose::DeadLeft;
	}
	
	PlayMontageSection(DeathMontage, Section);
	
}

void AEnemy::OnDeath()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(10.f);
}

void AEnemy::DirectionalHitReact(double HitAngle)
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

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	const auto Angle = CalculateImpactAngle(ImpactPoint);
	if (Attributes->IsAlive())
	{
		DirectionalHitReact(Angle);
	}
	else
	{
		DirectionalDeath(Angle);
		OnDeath();
	}

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(Attributes->IsAlive());
	}
	
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticleSystem, ImpactPoint);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	Attributes->TakeDamage(DamageAmount);
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercentage());
	}

	CombatTarget = EventInstigator->GetPawn();
	
	return DamageAmount;
}

