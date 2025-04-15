// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_DeactivateLeftWeapon.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Deactivate Left Weapon"))
class AAAMECHANICS_API UAnimNotify_DeactivateLeftWeapon : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
