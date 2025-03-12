// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ReplaceClip.h"
#include "..//Characters/NiceCharacter.h"

#include "..//DebugMacros.h"

void UAnimNotify_ReplaceClip::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner()) {
		if (AActor* Owner = MeshComp->GetOwner()) {
			if (auto* NiceCharacter = Cast<ANiceCharacter>(Owner)) {
                //show("ReplaceClip"); //! Debug
				NiceCharacter->ReleaseClip();
             }
        }
    }
}
