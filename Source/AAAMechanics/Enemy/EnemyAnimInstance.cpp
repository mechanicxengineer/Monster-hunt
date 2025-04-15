// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (EnemyRef == nullptr) {
        EnemyRef = Cast<AEnemy>(TryGetPawnOwner());
    }

    if (EnemyRef) {
        FVector Velocity{ EnemyRef->GetVelocity() };
        Velocity.Z = 0.f;
        Speed = Velocity.Size();

    }
}