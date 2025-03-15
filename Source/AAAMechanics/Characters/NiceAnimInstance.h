// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NiceAnimInstance.generated.h"

class ANiceCharacter;

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming		UMETA(DisplayName="Aiming"),
	EOS_Hip			UMETA(DisplayName="Hip"),
	EOS_Reloading	UMETA(DisplayName="Reloading"),
	EOS_InAir		UMETA(DisplayName="InAir"),

	EOS_Max 		UMETA(DisplayName="DefaultMax")
};

/**
 * 
 */
UCLASS()
class AAAMECHANICS_API UNiceAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	ANiceCharacter* NiceCharacter;

	/**	Character's movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;
	/** Whether or not the character is in th air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	/** Wheather or not the character is in moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	/** Offset yaw for strafing */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;
	/** Last offset yaw for strafing */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAiming;
	
	/** Yaw of the Character this frame, only update when standing still and not in air */
	float TIP_CharacterYaw;
	/** Yaw of the Character the previous frame, Only update when standing still and not in air */
	float TIP_CharacterYawLastFrame;
	
	/** Yaw of the Character this frame */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;
	
	/** Rotation curve value this frame */
	float RotationCurve;
	/** Rotation curve value last frame */
	float RotationCurveLastFrame;
	
	/** The pitch of the aim rotation, used for aim offset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
	float Pitch;
	
	/** true when reloading, used to prevent aim offset while reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;
	
	/** Offset state, used to determine which aim offset to use */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;
	
	/** Yaw of the character */
	FRotator CharacterRotation;
	/** Chacrater yaw last frame */
	FRotator CharacterRotationLastFrame;
	/** Yaw delta used for leaning in the running blendspce */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	float YawDelta;
	
	/** true when crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;
	
	/** Change the recoil weight based on turing in place and aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;
	
	/** True turning in place */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;

public:

	UNiceAnimInstance();
	
	/**	Initializes the animation instance */
	virtual void NativeInitializeAnimation() override;
	
	/**	Called every frame */
	UFUNCTION(BlueprintCallable)
	virtual void UpdateAnimationProperties(float DeltaSeconds);
	
protected:
	/** handle turning in place variables */
	void TurnInPlace();

	/** Handle calculation of leaning */
	void Lean(float deltaTime);

};
