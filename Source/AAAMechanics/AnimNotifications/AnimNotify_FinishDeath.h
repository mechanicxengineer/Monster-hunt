// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FinishDeath.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Finish Death"))
class AAAMECHANICS_API UAnimNotify_FinishDeath : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent*, UAnimSequenceBase*) override;

};
