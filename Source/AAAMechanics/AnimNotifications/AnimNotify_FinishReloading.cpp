// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FinishReloading.h"
#include "..//Characters/NiceCharacter.h"

//  Debug
#include "..//DebugMacros.h"

void UAnimNotify_FinishReloading::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //show("Reloading Finished");     //!  Debug
    if (MeshComp && MeshComp->GetOwner()) {
        if (auto* Owner = MeshComp->GetOwner()) {
            if (auto* NiceCharacter = Cast<ANiceCharacter>(Owner)) {
                NiceCharacter->FinishReloading();
            }
        }
    }
}