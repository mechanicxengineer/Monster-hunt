// Fill out your copyright notice in the Description page of Project Settings.

//	Engine includes
#include "NiceCharacter.h"

#include "Camera//CameraComponent.h"

#include "GameFramework//SpringArmComponent.h"
#include "GameFramework//CharacterMovementComponent.h"

#include "Engine//SkeletalMeshSocket.h"

#include "Kismet//GameplayStatics.h"

#include "Sound//SoundCue.h"

#include "Particles//ParticleSystemComponent.h"

#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

//	Custom includes
#include "..//Items//Item.h"
#include "..//Items//Weapons//Weapon.h"

//	Debug includes
#include "..//Common//advlog.h"
#include "..//DebugMacros.h"

// Sets default values
ANiceCharacter::ANiceCharacter() : 
	  /** Base rates for turning/looking up */
	  BaseTurnRate(45.0f),
	  BaseLookupRate(45.0f),
	  /** Turn rates for aiming/not aiming  */
	  HipTurnRate(90.0f),
	  HipLookUpRate(90.0f),
	  AimingTurnRate(20.0f),
	  AimingLookUpRate(20.0f),
	  /** Mouse look sensitivity scale factors */
	  MouseHipTurnRate(1.0f),
	  MouseHipLookUpRate(1.0f),
	  MouseAimingTurnRate(0.2f),
	  MouseAimingLookUpRate(0.2f),
	  /** true when aiming the weapon */
	  bAiming(false),
	  /** Camera field of view values */
	  CameraDefaultFieldOfView(0.0f),
	  CameraZoomedFieldOfView(35.0f),
	  CameraCurrentFieldOfView(0.0f),
	  ZoomInterpSpeed(20.0f),
	  //	Spread factors
	  CrosshairSpreadMultiplier(0.0f),
	  CrosshairVelocityFactor(0.0f),
	  CrosshairInAirFactor(0.0f),
	  CrosshairAimingFactor(0.0f),
	  CrosshairShootingFactor(0.0f),
	  //	Automatic fire variables
	  bFireButtonPressed(false),
	  bShouldFire(true),
	  AutomaticFireRate(0.1f),
	  //	Timer variables
	  ShootTimeDuration(0.05f),
	  bFiringBullet(false),
	  //	Trace variables
	  bShouldTraceForItems(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create a camera boom(pulls in towards the character)  */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f;										//	The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true;									//	Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 70.0f);


	/**	Create a follow camera */
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	//	Attach camera to end of boom
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;								//	Camera does not rotate relative to arm

	//	Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;					//	Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator{ 0.0f, 540.0f, 0.0f };		//	At this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void ANiceCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (FollowCamera) {
		CameraDefaultFieldOfView = GetFollowCamera()->FieldOfView;
		CameraCurrentFieldOfView = CameraDefaultFieldOfView;
	}
	/** Spawn default weapon and equip it */
	EquipWeapon(SpawnDefaultWeapon());
}

// Called every frame
void ANiceCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/** Handle interpolation for zoom when aiming */
    CameraInterZoom(DeltaTime);
	/** Change look sensitivity based on aiming */
    SetLookRates();
	/** Calculate crosshair spread */
	CalculateCrosshairSpread(DeltaTime);

    TraceForItem();
}

void ANiceCharacter::TraceForItem()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemHitResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemHitResult, HitLocation);
		if (ItemHitResult.bBlockingHit) {
			TraceHitItem = Cast<AItem>(ItemHitResult.Actor);
			if (TraceHitItem && TraceHitItem->GetPickupWidget()) {
				/** Show Item's visibility  */
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}
			/** We hit an AItem last frame */
			if (TraceHitItemLastFrame) {
				if (TraceHitItemLastFrame != TraceHitItem) {
					/** We are hitting a different aitem this frame from last frame */
					/** Or AItem is null */
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}
			/** store a reference to the item for next frame */
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame) {
		/** 
		  * No longet overlapping any items 
		  * Item last frame should not show widget
		  */
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
}

AWeapon* ANiceCharacter::SpawnDefaultWeapon() 
{
	/** check the TSubclassOf variable */
	if (DefaultWeaponClass) {
		/** spawn the weapon */
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void ANiceCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip) {
		/** Get the hand socket */
		const USkeletalMeshSocket* RightHandSocket{ GetMesh()->GetSocketByName("right_hand_socket") };
		if (RightHandSocket) {
			/** Attach the weapon to the hand socket */
			RightHandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		/** Set the equipped weapon to the weapon that was passed in */
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_EQUIPPED);
	}
}

void ANiceCharacter::DropWeapon()
{
	if (EquippedWeapon) {
		FDetachmentTransformRules detachmentTransformRules{ EDetachmentRule::KeepWorld, true };
		EquippedWeapon->GetItemMesh()->DetachFromComponent(detachmentTransformRules);
		
		EquippedWeapon->SetItemState(EItemState::EIS_FALLING);
		EquippedWeapon->ThrowWeapon();
	}
}

void ANiceCharacter::SelectButtonPressed()
{
	if (TraceHitItem) {
		SwapWeapon(Cast<AWeapon>(TraceHitItem));
	}
}

void ANiceCharacter::SelectButtonReleased()
{

}

void ANiceCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void ANiceCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootingResetTimer, this, 
		&ANiceCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

void ANiceCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

float ANiceCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

// Called to bind functionality to input
void ANiceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ANiceCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANiceCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANiceCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANiceCharacter::LookupAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &ANiceCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ANiceCharacter::LookUp);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &ANiceCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &ANiceCharacter::FireButtonReleased);
	
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &ANiceCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &ANiceCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &ANiceCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &ANiceCharacter::SelectButtonReleased);
	
}

void ANiceCharacter::MoveForward(float _value)
{
	if ((Controller != nullptr) && _value != 0.0f) {
		/** find out which way is forward */
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, _value);
	}
}

void ANiceCharacter::MoveRight(float _value)
{
	if ((Controller != nullptr) && _value != 0.0f) {
		/** find out which way is forward */
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, _value);
	}

}

void ANiceCharacter::TurnAtRate(float rate)
{
	/** calculate delta for this frame from the rate information */
	//	deg/sec * sec/frame
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANiceCharacter::LookupAtRate(float rate)
{
	//	deg/sec * sec/frame
	AddControllerPitchInput(rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

void ANiceCharacter::Turn(float value)
{
	float TurnScaleFactor{};
	if (bAiming) {
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else {
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(value * TurnScaleFactor);
}

void ANiceCharacter::LookUp(float value)
{
	float LookUpScaleFactor{};
	if (bAiming) {
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else {
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(value * LookUpScaleFactor);
}

void ANiceCharacter::FireWeapon()
{
	if (FireSound) {
		//SHOW("Fire Weapon");
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	const USkeletalMeshSocket* BarrelSocket{ GetMesh()->GetSocketByName("barrelSocket") };
	if (BarrelSocket) {
		const FTransform SocketTransform{ BarrelSocket->GetSocketTransform(GetMesh()) };
		if (MuzzleFlash) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}
		
		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd) {
			if (ImapctParticles) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImapctParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam{ UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform) };
			if (Beam) {
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}

	auto AnimInstance{ GetMesh()->GetAnimInstance() };
	if (AnimInstance && HipFireMontage) {
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"), HipFireMontage);
	}
	/** Start bullet fire timer for crosshair */
	StartCrosshairBulletFire();
}

bool ANiceCharacter::GetBeamEndLocation(const FVector &MuzzleSocketLocation, FVector &OutBeamLocation)
{
	/** Check for crosshair trace hit */
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);
	if (bCrosshairHit) {
		/** Tentative beam location - still need to trance from gun */
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else {	//	no crosshair trace hit
		//	Outbeantrance is the end location for the line trance
	}
	//	Perform a second trace, this time from the gun barrel
	FHitResult WeaponHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(WeaponHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
	if (WeaponHit.bBlockingHit) {				//	object between barrel and beam end point ?	
		OutBeamLocation = WeaponHit.Location;
		return true;
	}
	return false;

	/** Get current size of the viewport */
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	/** Get screen space location of crosshair */
	FVector2D CrosshairLocation{ ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f };
	FVector CrosshairWorldLocation;
	FVector CrosshairWorldDirection;
	
	/** Get world position and direction of crosshairs */
	bool bScreenToWord = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), 
	CrosshairLocation, CrosshairWorldLocation, CrosshairWorldDirection);

	if (bScreenToWord) {							//	was the deprojection successful ?
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldLocation };
		const FVector End{ CrosshairWorldLocation + CrosshairWorldDirection * 50'000.0f };
		/** Set bean end point to line trace end point */
		OutBeamLocation = End;
		/**	Trace outward form crosshair world location */
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);
		if (ScreenTraceHit.bBlockingHit) {			//	did this hit something ?
			/** Beam end point is now hit location */
			OutBeamLocation = ScreenTraceHit.Location;
			if (ImapctParticles) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImapctParticles, ScreenTraceHit.Location);
			}
		}
	}
}

void ANiceCharacter::AimingButtonPressed()
{
	bAiming = true;
	//GetCharacterMovement()->MaxWalkSpeed = 150.0f;
}

void ANiceCharacter::AimingButtonReleased()
{
	bAiming = false;
	//GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void ANiceCharacter::CameraInterZoom(float DeltaTime)
{
	/**	Interpolate to zoomed fov when aiming */
	if (bAiming)
	{ // Button pressed
		CameraCurrentFieldOfView = FMath::FInterpTo(CameraCurrentFieldOfView, CameraZoomedFieldOfView, DeltaTime, ZoomInterpSpeed);
	}
	else
	{ // Button released
		CameraCurrentFieldOfView = FMath::FInterpTo(CameraCurrentFieldOfView, CameraDefaultFieldOfView, DeltaTime, ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFieldOfView);
}

void ANiceCharacter::SetLookRates()
{
	if (bAiming) {
		BaseTurnRate = AimingTurnRate;
		BaseLookupRate = AimingLookUpRate;
	}
	else {
		BaseTurnRate = HipTurnRate;
		BaseLookupRate = HipLookUpRate;
	}
}

void ANiceCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	/** Calculate crosshair spread */
	FVector2D WalkSpeedRange{ 0.0f, 600.0f };
	FVector2D VelocityMultiplierRange{ 0.0f, 1.0f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.0f;
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	/** Calculate crosshair in air factor */
	if (GetCharacterMovement()->IsFalling()) {			//	are we in the air ?
		/** Spread factor is 2.25 when in air */
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else {												//	we are on the ground
		/** Shrink factor is 0.0 when on the ground */
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.0f, DeltaTime, 30.0f);
	}

	/** Calculate crosshair aiming factor */
	if (bAiming) {
		/** Shrink factor is 0.0 when aiming */
		CrosshairAimingFactor = FMath::FInterpTo(CrosshairAimingFactor, 0.6f, DeltaTime, 30.0f);
	}
	else {
		/** Spread factor is 0.5 when not aiming */
		CrosshairAimingFactor = FMath::FInterpTo(CrosshairAimingFactor, 0.0f, DeltaTime, 30.0f);
	}

	if (bFiringBullet) {
		/** Spread factor is 0.5 when firing */
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.5f, DeltaTime, 60.0f);
	}
	else {
		/** Shrink factor is 0.0 when not firing */
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 60.0f);
	}
	
	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor 
		- CrosshairAimingFactor + CrosshairShootingFactor;
}

void ANiceCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void ANiceCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void ANiceCharacter::StartFireTimer()
{
	if (bShouldFire) {
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, 
			&ANiceCharacter::AutoFireReset, AutomaticFireRate, false);
	}
}

void ANiceCharacter::AutoFireReset()
{
	bShouldFire = true;
	if (bFireButtonPressed) {
		StartFireTimer();
	}
}

bool ANiceCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	/** Get viewport size */
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	/** Get screen space location of crosshair */
	FVector2D CrosshairLocation{ ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f };
	FVector CrosshairWorldLocation;
	FVector CrosshairWorldDirection;

	/** Get world position and direction of crosshairs */
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldLocation, CrosshairWorldDirection);
	
	if (bScreenToWorld) {
		const FVector Start{ CrosshairWorldLocation };
		const FVector End{ CrosshairWorldLocation + CrosshairWorldDirection * 50'000.0f };
		OutHitLocation  = End;

		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit) {
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void ANiceCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0) {
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else {
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

