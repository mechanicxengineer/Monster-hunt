// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NicePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AAAMECHANICS_API ANicePlayerController : public APlayerController
{
	GENERATED_BODY()

	/** Reference to the Overall Hud overlay Blueprint class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"));
	TSubclassOf<class UUserWidget> NiceOverlayClass;

	/** Variable to hold the hud overlay widget after creating it */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess = "true"));
	UUserWidget* NiceOverlay;

public:
	ANicePlayerController();

protected:
	virtual void BeginPlay() override;
};
