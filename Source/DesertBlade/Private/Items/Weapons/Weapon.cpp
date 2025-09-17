// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "DesertBladeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MetasoundSource.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
	: Hitbox(CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox")))
{
	Hitbox->SetupAttachment(GetRootComponent());
	Hitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Hitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Hitbox->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::AttachMeshToSocket(USceneComponent* Parent, const FName& Socket)
{
	Mesh->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
}

void AWeapon::Equip(USceneComponent* Parent, const FName& Socket, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(Parent, Socket);
	ItemState = EItemState::Equipped;
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
	if (ItemParticleSystem)
	{
    	ItemParticleSystem->Deactivate();
	}
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	FHitResult OutBoxHit;
	
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector{11.f, 9.f, 3.f},
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		OutBoxHit,
		true
		);

	if (OutBoxHit.GetActor())
	{
		UGameplayStatics::ApplyDamage(OutBoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

		if (IHitInterface* HitActor = Cast<IHitInterface>(OutBoxHit.GetActor()))
		{
			IHitInterface::Execute_GetHit(OutBoxHit.GetActor(), OutBoxHit.ImpactPoint);
		}
		IgnoreActors.AddUnique(OutBoxHit.GetActor());

		CreateFields(OutBoxHit.ImpactPoint);
	}

	// if (OutBoxHit.GetActor())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *OutBoxHit.GetActor()->GetName());
	// }
	
}
