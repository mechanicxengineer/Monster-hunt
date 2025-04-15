// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ActivateLeftWeapon.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Activate Left Weapon"))
class AAAMECHANICS_API UAnimNotify_ActivateLeftWeapon : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
