// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ActivateRightWeapon.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Activate Right Weapon"))
class AAAMECHANICS_API UAnimNotify_ActivateRightWeapon : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
