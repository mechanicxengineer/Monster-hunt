// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XPComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AAAMECHANICS_API UXPComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	int CurrentLevel;
	int MaxLevel;
	float CurrentXP;
	float CurrentMaxXP{ 1000.0f };
	float CurrentPercentage;
	float StoredXP;

	UXPComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CalculatePercentage();
	void CalculateMaxXP();
	UFUNCTION(BlueprintCallable)void AddXP(float AddedXP);
	void LevelUp();
};
