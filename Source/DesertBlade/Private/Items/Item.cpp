// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

#include "DesertBladeCharacter.h"
#include "Components/SphereComponent.h"
#include "DesertBlade/DebugMacros.h"
#include "DesertBlade/DesertBlade.h"
#include "IO/IoStoreOnDemand.h"
#include "NiagaraComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Mesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	ItemParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleEffect"));
	ItemParticleSystem->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * Frequency);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * Frequency);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto* Character = Cast<ADesertBladeCharacter>(OtherActor))
	{
		Character->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (auto* Character = Cast<ADesertBladeCharacter>(OtherActor))
	{
		Character->SetOverlappingItem(nullptr);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
		AddActorWorldRotation(FRotator(0.f, DeltaTime * 120.f, 0.f));
	}
}

