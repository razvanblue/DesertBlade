// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class DESERTBLADE_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:

	virtual void InitWidget() override;
	
	UFUNCTION(BlueprintCallable)
	void SetHealthPercent(float Percent);

private:
	UPROPERTY(Transient)
	class UHealthBar* HealthBarWidget;
};
