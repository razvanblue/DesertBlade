// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "Algo/Copy.h"
#include "AIController.h"
#include "DesertBladeCharacter.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "DesertBlade/DebugMacros.h"
#include "Items/Weapons/Weapon.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarComponent"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 4000.f;
	
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercentage());
	HealthBarWidget->SetVisibility(false);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
		
	}
	
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (State == EEnemyState::Dead) return;

	if (State > EEnemyState::Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

bool AEnemy::InTargetRange(const AActor* Target, double Radius)
{
	const double SquaredDistance = (Target->GetActorLocation() - GetActorLocation()).SquaredLength();

	return SquaredDistance <= Radius * Radius;
}

void AEnemy::PatrolTimerFinished()
{
	if (PatrolTarget)
	{
		MoveToTarget(PatrolTarget);
	}
}

void AEnemy::MoveToTarget(const AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(30.f);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	ValidTargets.Reserve(PatrolTargets.Num());
	Algo::CopyIf(PatrolTargets, ValidTargets,
		[this](AActor* Target) { return Target != PatrolTarget.Get(); });
		
	if (ValidTargets.Num())
	{
		const int32 NewPatrolTargetIndex = FMath::RandRange(0, ValidTargets.Num() - 1);
		return ValidTargets[NewPatrolTargetIndex];
	}
	
	return nullptr;
}

void AEnemy::CheckCombatTarget()
{
	if (CombatTarget == nullptr) return;
	
	if (!InTargetRange(CombatTarget, CombatRadius))
	{	// Outside combat range, lose interest
		CombatTarget = nullptr;

		State = EEnemyState::Patrolling; UE_LOG(LogTemp, Warning, TEXT("Patrolling"));
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);
		
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);				
		}
	}
	else if (State != EEnemyState::Chasing && !InTargetRange(CombatTarget, AttackRadius))
	{	// In combat range, but outside attack range, chase character
		State = EEnemyState::Chasing; UE_LOG(LogTemp, Warning, TEXT("Chasing"));
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
	}
	else if (State != EEnemyState::Attacking && InTargetRange(CombatTarget, AttackRadius))
	{	// Inside attack range, attack character
		State = EEnemyState::Attacking; UE_LOG(LogTemp, Warning, TEXT("Attacking"));
		Attack();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (PatrolTarget && InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const double PatrolDelay = FMath::RandRange(PatrolDelayMin, PatrolDelayMax);
		GetWorldTimerManager().SetTimer(
			PatrolTimer, this, &AEnemy::PatrolTimerFinished, PatrolDelay);
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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

void AEnemy::Attack()
{
	PlayRandomMontageSection(AttackMontage);
}

void AEnemy::OnDeath()
{
	State = EEnemyState::Dead;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(10.f);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (State < EEnemyState::Chasing && SeenPawn->ActorHasTag(FName("Player")))
	{
		CombatTarget = SeenPawn;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		State = EEnemyState::Chasing; UE_LOG(LogTemp, Warning, TEXT("Chasing"));
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
	}
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
	State = EEnemyState::Chasing; UE_LOG(LogTemp, Warning, TEXT("Chasing"));
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	MoveToTarget(CombatTarget);
	
	return DamageAmount;
}

