// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_GrabClip.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Grab Clip Notification"))
class AAAMECHANICS_API UAnimNotify_GrabClip : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
