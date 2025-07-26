// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Begin Play!"));
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, TEXT("Item OnScreen Message!"), true, FVector2D::UnitVector * 2);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

