// Fill out your copyright notice in the Description page of Project Settings.


#include "XPWidget.h"
#include "Kismet//GameplayStatics.h"
#include "Components//TextBlock.h"
#include "Components//ProgressBar.h"
#include "XPComponent.h"
#include "..//..//AaaCharacter.h"

void UXPWidget::NativeConstruct()
{
    Super::NativeConstruct();
    XPComponent = Cast<AAaaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetXPComponent();
}

void UXPWidget::UpdateXP()
{
    if (XPComponent) {
        if (CurrentXPText) {
            CurrentXPText->SetText(FText::AsNumber(XPComponent->CurrentXP));
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("CurrentXPText is null"));
        }

        if (MaxXP) {
            MaxXP->SetText(FText::AsNumber(XPComponent->CurrentMaxXP));
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("MaxXPText is null"));
        }

        if (XPBar) {
            XPBar->SetPercent(XPComponent->CurrentPercentage);
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("XPBar is null"));
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("XPComponent is null in UpdateXP"));
    }
}