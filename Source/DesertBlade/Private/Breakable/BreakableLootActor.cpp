// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableLootActor.h"

#include "Items/Treasure.h"

void ABreakableLootActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);
}

void ABreakableLootActor::OnBreak_Implementation()
{
	Super::OnBreak_Implementation();

	SpawnLoot();
}

void ABreakableLootActor::SpawnLoot()
{
	UWorld* World = GetWorld();
	
	if (World && LootClasses.Num())
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += 50.f;
		
		const int32 Selection = FMath::RandRange(0, LootClasses.Num() - 1);
		World->SpawnActor<ATreasure>(LootClasses[Selection], SpawnLocation, GetActorRotation());
	}
}
