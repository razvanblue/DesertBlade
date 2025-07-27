// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DesertBlade/DebugMacros.h"
#include "DesertBlade/DesertBlade.h"
#include "IO/IoStoreOnDemand.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;
	float DeltaZ = Amplitude * FMath::Sin(RunningTime * Frequency);
	AddActorWorldOffset(FVector{0.f, 0.f, DeltaZ});
	
	DRAW_SPHERE_SINGLE_FRAME(GetActorLocation());
	DRAW_DEBUG_VECTOR_SINGLE_FRAME(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 120.f);
}

