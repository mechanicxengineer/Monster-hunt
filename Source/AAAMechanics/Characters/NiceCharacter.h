// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NiceCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class AAAMECHANICS_API ANiceCharacter : public ACharacter
{
	GENERATED_BODY()

	/**	CamerBoom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	/** Camera that follows the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Base trun rate, in deg/sec, Other scaling may affect final turn rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookupRate;

	/** Sound to play when the character fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/** Particle system to spawn when the character fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/** Montage to play when the character fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

public:
	// Sets default values for this character's properties
	ANiceCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**	Called for forward/backwards input */
	void MoveForward(float _value);
	/** Called for right/left input */
	void MoveRight(float _value);

	/**
	 * 	Called via input to turn at a given rate.
	 *	@param rate	This is a normailized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float rate);
	
	/**
	 * 	Called via input to look up/down at a given rate.
	 * 	@param rate	This is a normailized rate, i.e. 1.0 means 100% of desired rate
	 */
	void LookupAtRate(float rate);

	/**
	 *  Called when the fire button is pressed
	 */
	void FireWeapon();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*************	 GETTTERS	***************/
	/**	Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return	CameraBoom; }
	/** Returns FollowCamera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/***************************************/

};
