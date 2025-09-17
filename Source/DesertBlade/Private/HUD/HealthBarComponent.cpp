// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"

void UHealthBarComponent::InitWidget()
{
	Super::InitWidget();

	HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
}

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
