// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NiceAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class AAAMECHANICS_API UNiceAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class ANiceCharacter* NiceCharacter;

	/**	Character's movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;
	/** Whether or not the character is in th air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	/** Wheather or not the character is in moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

public:
	/**	Initializes the animation instance */
	virtual void NativeInitializeAnimation() override;

	/**	Called every frame */
	UFUNCTION(BlueprintCallable)
	virtual void UpdateAnimationProperties(float DeltaSeconds);
};
