// Fill out your copyright notice in the Description page of Project Settings.


#include "NiceAnimInstance.h"
#include "NiceCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UNiceAnimInstance::NativeInitializeAnimation()
{
    NiceCharacter = Cast<ANiceCharacter>(TryGetPawnOwner());
}

void UNiceAnimInstance::UpdateAnimationProperties(float DeltaSeconds)
{
    if (NiceCharacter == nullptr) {
        NiceCharacter = Cast<ANiceCharacter>(TryGetPawnOwner());
    }
    if (NiceCharacter) {
        /** Get the speed of the character form velocity */
        FVector Velocity{ NiceCharacter->GetVelocity() };
        Velocity.Z = 0;
        Speed = Velocity.Size();

        /** Is the character in the air ? */
        bIsInAir = NiceCharacter->GetCharacterMovement()->IsFalling();
        /** Is the character accelerating ? */
        bIsAccelerating = NiceCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;
    }
}
