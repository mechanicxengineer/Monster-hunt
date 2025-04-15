// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FinishNiceCharDeath.h"
#include "..//Characters//NiceCharacter.h"

void UAnimNotify_FinishNiceCharDeath::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	if (MeshComp && MeshComp->GetOwner()) {
		if (auto* Char = Cast<ANiceCharacter>(MeshComp->GetOwner())) {
			Char->FinishDeath();
		}
	}
}