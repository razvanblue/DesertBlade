// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Engine/Engine.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
 
	GeometryCollection->SetNotifyBreaks(true);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::Crack);
	Level1BoneCount = GetLevel1BoneCount();
	DestroyThreshold *= Level1BoneCount;
}

int32 ABreakableActor::GetLevel1BoneCount() const
{
	const auto GC = GeometryCollection->RestCollection->GetGeometryCollection();
	
	int32 BoneCount = 1;
	const TManagedArray<int32>& Levels =
		GC->GetAttribute<int32>("Level", FGeometryCollection::TransformGroup);
	const int32 TotalBoneCount = GC->NumElements(FGeometryCollection::TransformGroup);
	
	for (; BoneCount < TotalBoneCount && Levels[BoneCount] < 2; BoneCount++)
	{
	}

	return BoneCount - 1;
}

void ABreakableActor::Crack(const FChaosBreakEvent& BreakEvent)
{
	if (bBroken)
	{
		return;
	}
	
	bool IsLevel1Bone = BreakEvent.Index < Level1BoneCount;
	
	if (IsLevel1Bone && ++BrokenPiecesCount > DestroyThreshold)
	{
		Break();
	}
}

void ABreakableActor::OnBreak_Implementation()
{

}

void ABreakableActor::Break()
{
	bBroken = true;
	FGCCollisionProfileScopedTransaction Transaction(GeometryCollection);
	for (int i = 0; i < GeometryCollection->GetNumElements(FGeometryCollection::TransformGroup); ++i)
	{
		GeometryCollection->SetParticleCollisionProfileName(i, "DestructibleDebris", Transaction);
	}
	OnBreak();
	
	SetLifeSpan(LifespanAfterBreak);
}

