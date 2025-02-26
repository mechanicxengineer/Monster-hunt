// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XPWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class AAAMECHANICS_API UXPWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UXPComponent* XPComponent;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CurrentXPText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MaxXP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* XPBar;

	virtual void NativeConstruct() override;
	void UpdateXP();
};
