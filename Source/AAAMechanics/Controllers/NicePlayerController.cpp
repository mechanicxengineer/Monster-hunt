// Fill out your copyright notice in the Description page of Project Settings.


#include "NicePlayerController.h"
#include "Blueprint//UserWidget.h"

ANicePlayerController::ANicePlayerController()
{

}

void ANicePlayerController::BeginPlay()
{
	Super::BeginPlay();

    /** Check our HudOverlayClass TSubclassOf variable */
    if (NiceOverlayClass) {
        NiceOverlay = CreateWidget<UUserWidget>(this, NiceOverlayClass);
        if (NiceOverlay) {
            NiceOverlay->AddToViewport();
            NiceOverlay->SetVisibility(ESlateVisibility::Visible);
        }
    }
}