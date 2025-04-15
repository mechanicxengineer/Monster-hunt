// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class AAAMECHANICS_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	/** Lateral Movement Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemy* EnemyRef;

public:
	UFUNCTION(BlueprintCallable) 
	void UpdateAnimationProperties(float DeltaTime);

};
