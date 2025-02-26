// Fill out your copyright notice in the Description page of Project Settings.


#include "XPComponent.h"
#include "XPWidget.h"

UXPComponent::UXPComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize default values
	CurrentXP = 0.0f;
	StoredXP = 0.0f;
	CurrentMaxXP = 100.0f;
	CurrentPercentage = 0.0f;
}


// Called when the game starts
void UXPComponent::BeginPlay()
{
	Super::BeginPlay();

	CalculatePercentage();
}

// Called every frame
void UXPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    float InterpedXP = FMath::FInterpTo(CurrentXP, StoredXP, DeltaTime, 5.0f);
    CurrentXP = FMath::CeilToInt(InterpedXP);
    CalculatePercentage();
	
    if (CurrentXP >= CurrentMaxXP)
    {
        LevelUp();
    }
}

void UXPComponent::CalculatePercentage()
{
    CurrentPercentage = CurrentXP / CurrentMaxXP;
}

void UXPComponent::CalculateMaxXP()
{
    CurrentMaxXP = FMath::CeilToInt(CurrentMaxXP * 1.5f);
}

void UXPComponent::AddXP(float AddedXP)
{
    StoredXP += AddedXP;
}

void UXPComponent::LevelUp()
{
	if (CurrentLevel >= MaxLevel) {
		CurrentXP = CurrentMaxXP;
		StoredXP = CurrentMaxXP;
	}
	else {
		CurrentLevel++;
		float _remainingXP = StoredXP - CurrentMaxXP;
		CalculateMaxXP();
		CurrentXP = 0.0f;
		StoredXP = 0.0f;
		AddXP(_remainingXP);
	}
}