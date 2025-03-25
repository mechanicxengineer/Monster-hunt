// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "..//AmmoType.h"
#include "NiceCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState  : uint8
{
	ECS_Unoccupied						UMETA(DisplayName="Unoccupied"),
	ECS_FireTimerInProgress 			UMETA(DisplayName="FireTimerInProgress"),
	ECS_Reloading						UMETA(DisplayName="Reloading"),
	ECS_Equipping						UMETA(DisplayName="Equipping"),

	ECS_Max 							UMETA(DisplayName="DefaultMax")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	/** Scene component to use for this location for interping */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	/** Number of items interping to/at this scene comp location */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UCLASS()
class AAAMECHANICS_API ANiceCharacter : public ACharacter
{
	GENERATED_BODY()

	/**	CamerBoom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	/** Camera that follows the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base trun rate, in deg/sec, Other scaling may affect final turn rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookupRate;
	
	/** Turn rate while not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;
	/** Look up rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;
	/** Turn rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;
	/** Aiming up rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;
	/** Scale factor for mouse look sensitivity. Trun rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true", 
		ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;
	/** Scale factor for mouse look sensitivity. Look up rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true", 
		ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;
	/** Scale factor for mouse look sensitivity. Look up rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true", 
		ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;
	/** Scale factor for mouse look sensitivity. Look up rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true", 
		ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;


	/** Sound to play when the character fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/** Particle system to spawn when the character fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;
	
	/** Impact particle to spawn when we hit a surface */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImapctParticles;
	
	/** Smoke trails for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BeamParticles;

	/** Montage to play when the character fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/** Montage to play when the character fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* EquipMontage;

	/** Wheather or not the character is aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;
	
	/** Default camera field of view value */
	float CameraDefaultFieldOfView;
	/** Field of view value for when zoomed in */
	float CameraZoomedFieldOfView;
	/** Current field of view */
	float CameraCurrentFieldOfView;
	/** Interp speed for zooming in and out */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/** Determines the spread of the crosshairs */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	/** Velocity component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;
	/** Spread factor when in the air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;
	/** Spread factor when aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimingFactor;
	/** Spread factor when shooting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;
	
	/** sets a timer between gunshots */
	FTimerHandle AutoFireTimer;
	/** left mouse button or right trigger pressed */
	bool bFireButtonPressed;
	/** True when we can fire, False when waiting for the timer */
	bool bShouldFire;
	/** Rate of automatic gunfire */
	float AutomaticFireRate;

	/** Timer handle for managing the fire rate */
	FTimerHandle CrosshairShootingResetTimer;
	float ShootTimeDuration;
	bool bFiringBullet;
	/** True if we should trace every frame for items */
	bool bShouldTraceForItems;
	/** Number of overlapped AItems */
	int8 OverlappedItemCount;

	/** The AItem we hit last frame */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;
	
	/** Currently equipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquippedWeapon;
	
	/** Set this in Blueprint for the default weapon class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;
	/** The item currently hit by our trance in TranceForItems (could be null) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;
	
	/** Distance outward from the camera for the interp destination */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;
	/** Distance upward from the camera for the interp destination */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;
	
	/** Map to keep track of ammo of the different ammo types */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	
	/** Starting amount of 9mm ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;
	/** Starting amount of ar ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;
	
	/** Combat state, can only fire or reload if Unoccupied */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;
	
	/** Montage for reload animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadingMontage;
	
	/** Transform of the clip when we first grab the during reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;
	
	/** scene component to attach to the character's hand during reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;
	
	/** ture when crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;
	/** regular movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;
	/** crouch movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingMovementSpeed;
	
	/** Current half height of the capsule` */
	float CurrentCapsuleHalfHeight;
	/** Half height of the capsule when not crouching */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;
	/** Half height of the capsule when crouching */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;
	
	/** Ground friction while not crouching */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;
	/** ground friction while crouching */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;
	
	/** Used fpr knowing when the aiming button is pressed */
	bool bAimingButtonPressed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

	/** Array of interp location structs */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;
	
	/** Time to wait before we can play another pickup sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;
	/** Time to wait before we can play another equip sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;
	
	/** An Array items for our inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;
	
	const int32 INVENTORY_MAX_SIZE{ 6 };
	
	/** the index for the currenty highlighted slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlotIndex;
	
	/** Delegate for sending slot information to inventoryBar when equipping */
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
	FOnEquipItemDelegate OnEquipItem;
	
	/** Delegate for sending slot information for playing the icon anations */
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate OnHighlightSlot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 HightlightSlot;

	/** Functions  */
	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

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
	 *  Rotate controller based on mouse x movement
	 * 	@param Value This input value from the mouse
	 */
	void Turn(float value);

	/**
	 *  Rotate controller based on mouse y movement
	 * 	@param Value This input value from the mouse
	 */
	void LookUp(float value);

	/**
	 *  Called when the fire button is pressed
	 */
    void FireWeapon();
    void PlayGunFireMontage();
    void SendBullet();
    void PlayFireSound();
    bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);
	/** Set bAiming to true of false with button press */
	void AimingButtonPressed();
	void AimingButtonReleased();
	void CameraInterZoom(float DeltaTime);
	/** Set look rates based on aiming or not */
	void SetLookRates();
	void CalculateCrosshairSpread(float DeltaTime);

	void FireButtonPressed();
	void FireButtonReleased();
	void StartFireTimer();
	UFUNCTION() void AutoFireReset();

	/** Line trace for items under the crosshairs */
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);
	/** trace for item if Overlapped Item count > 0*/
	void TraceForItem();
	/** Spawns a default weapon and equips it */
	AWeapon* SpawnDefaultWeapon();
	/** Equip the weapon and attaches it to the mesh */
	void EquipWeapon(AWeapon* weaponToEquip, bool bSwapping = false);
	/** Detach weapon and let it fall to the ground */
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	/** Drops currently equipped weapon and equips TranceHitItem */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/** Initialize the ammo map with ammo values */
	void InitializeAmmoMap();
	/** Check to make sure our character has ammo */
	bool WeaponHasAmmo();

	/** Bound to the r key */
	void ReloadButtonPressed();
	/** Handle Reloading of the weapon */
	void ReloadWeapon();

	/** checks to use if we have ammo of the EquippedWeapon's ammo type */
	bool CarryingAmmo();

	void CrouchButtonPressed();

	virtual void Jump() override;

	/** Inter calsule half height when crouching/ standing */
	void InterpCapsuleHalfHeight(float DeltaTime);

	void Aim();
	void StopAiming();

	void PickupAmmo(class AAmmo* Ammo);
	void InitializeInterpLocations();
	
	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);
	int32 GetEmptyInventorySlot();


public:	
	// Called every frame
    virtual void Tick(float DeltaTime) override;
    void StartCrosshairBulletFire();
	UFUNCTION()
	void FinishCrosshairBulletFire();

    // Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** Adds/subtracts to/from OverlappedItemCount and updates bShouldTrceForItem */
	void IncrementOverlappedItemCount(int8 Amount);
	void GetPickupItem(AItem* Item);

	void FinishReloading();
	/** called from animNotity_grabClip*/
	UFUNCTION(BlueprintCallable)
	void GrabClip();
	/** called from animNotity_ReplaceClip*/
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();
	
	/** No longer needed; Aitem has GetInterpLocation */
	//FVector GetCameraInterpolatedLocation();
	
	FInterpLocation GetInterpLocation(int32 Index);
	/** return the index in interplocations array with the lowest itemcount */
	int32 GetInterpLocationIndex();
	void IncrementInterpLocationItemCount(int32 Index, int32 Amount);
	
	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

	void HighlightInventorySlot();
	void UnhighlightInventorySlot();
	
	/*************	 GETTTERS	***************/
	/**	Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return	CameraBoom; }
	/** Returns FollowCamera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns bAiming */
	FORCEINLINE bool GetAiming() const { return bAiming; }
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayEquipSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayPickupSound; }
	

	/*****************************************/
	void SetUnoccupied();
};
