// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ActivateLeftWeapon.h"
#include "..//Enemy/Enemy.h"

void UAnimNotify_ActivateLeftWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner()) {
		if (AActor* Owner = MeshComp->GetOwner()) {
			if (auto* Ref = Cast<AEnemy>(Owner)) {
				Ref->ActivateLeftWeapon();
        	}
		}
    }
}