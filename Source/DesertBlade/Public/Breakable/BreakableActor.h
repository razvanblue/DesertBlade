// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class DESERTBLADE_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	

	ABreakableActor();

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:

	virtual void BeginPlay() override;

	/**
	 * Called by chaos OnChaosBreakEvent
	 * @param BreakEvent Break Event that triggered this call
	 */
	UFUNCTION()
	void Crack(const FChaosBreakEvent& BreakEvent);

	/**
	 * Called by Break() before setting actor lifespan
	 * @note Executed once when BreakThreshold has been reached
	 */
	UFUNCTION(BlueprintNativeEvent)
	void OnBreak();

	/**
	 * Geometry Collection
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bBroken;

private:
	
	FORCEINLINE void Break();

	FORCEINLINE int32 GetLevel1BoneCount() const;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 BrokenPiecesCount = 0;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 Level1BoneCount;

	/**
	 * Percentage of bones needed to break
	 * @note Gets converted to the actual number of bones on BeginPlay
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BreakProperties, meta = (
		AllowPrivateAccess = "true",
		ClampMin = "0.1",
		ClampMax = "1.0"
	))
	float DestroyThreshold = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BreakProperties, meta = (
		AllowPrivateAccess = "true"))
	float LifespanAfterBreak = 10.f;
};
