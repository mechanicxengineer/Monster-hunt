// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FinishReloading.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Finish Reloading Notification"))
class AAAMECHANICS_API UAnimNotify_FinishReloading : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
