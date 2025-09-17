// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Breakable/BreakableActor.h"
#include "BreakableLootActor.generated.h"

/**
 * 
 */
UCLASS()
class DESERTBLADE_API ABreakableLootActor : public ABreakableActor
{
	GENERATED_BODY()
public:
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	
	virtual void OnBreak_Implementation() override;

	FORCEINLINE void SpawnLoot();

	UPROPERTY(EditAnywhere, Category = "Loot")
	TArray<TSubclassOf<class ATreasure>> LootClasses;
};
