// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FinishNiceCharDeath.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Finish Character Death"))
class AAAMECHANICS_API UAnimNotify_FinishNiceCharDeath : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent*, UAnimSequenceBase*) override;

};
