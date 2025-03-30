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
#include "PhysicalMaterials//PhysicalMaterial.h"

#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

//	Custom includes
#include "..//Items//Item.h"
#include "..//Items//Ammo.h"
#include "..//Items//Weapons//Weapon.h"
#include "..//AAAMechanics.h"

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
	  MouseAimingTurnRate(0.6f),
	  MouseAimingLookUpRate(0.6f),
	  /** true when aiming the weapon */
	  bAiming(false),
	  /** Camera field of view values */
	  CameraDefaultFieldOfView(0.0f),
	  CameraZoomedFieldOfView(25.0f),
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
	  //	Timer variables
	  ShootTimeDuration(0.05f),
	  bFiringBullet(false),
	  //	Trace variables
	  bShouldTraceForItems(false),
	  OverlappedItemCount(0),
	  //	Camera interp variables
	  CameraInterpDistance(250.0f),
	  CameraInterpElevation(65.0f),
	  //	Starting ammo amount
	  Starting9mmAmmo(85),
	  StartingARAmmo(120),
	  //	Combat variables
	  CombatState(ECombatState::ECS_Unoccupied),
	  //	Crouching variables
	  bCrouching(false),
	  BaseMovementSpeed(600.0f),
	  CrouchingMovementSpeed(300.0f),
	  StandingCapsuleHalfHeight(88.0f),
	  CrouchingCapsuleHalfHeight(44.0f),
	  BaseGroundFriction(2.0f),
	  CrouchingGroundFriction(100.0f),
	  //	Aiming variables
	  bAimingButtonPressed(false),
	  //	Pickup sound timer properties
	  bShouldPlayPickupSound(true),
	  bShouldPlayEquipSound(true),
	  PickupSoundResetTime(0.2f),
	  EquipSoundResetTime(0.2f),
	  /** Icon animation properties */
	  HightlightSlot(-1)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create a camera boom(pulls in towards the character)  */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.0f;										//	The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true;									//	Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 80.0f);


	/**	Create a follow camera */
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	//	Attach camera to end of boom
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;								//	Camera does not rotate relative to arm

	/** Don't rotate when the controller rotates. Let that just affect the camera. */
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;					//	Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator{ 0.0f, 540.0f, 0.0f };		//	At this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	/** Create hand scene component */
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>("HandSceneComponent");

	/** Create interpolation component */
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>("Weapon Interp Component");
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>("Interp Component 1");
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>("Interp Component 2");
	InterpComp2->SetupAttachment(GetFollowCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>("Interp Component 3");
	InterpComp3->SetupAttachment(GetFollowCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>("Interp Component 4");
	InterpComp4->SetupAttachment(GetFollowCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>("Interp Component 5");
	InterpComp5->SetupAttachment(GetFollowCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>("Interp Component 6");
	InterpComp6->SetupAttachment(GetFollowCamera());

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
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();
	EquippedWeapon->SetCharacter(this);

	/** Call the Initialize ammo map */
	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	/** create Finterplocation structs for each interp each location. add to array */
	InitializeInterpLocations();

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
	/** check overlappedItemCount, then trace for items */
    TraceForItem();
	/** Interpolate the capsule half height based on crouching/ standing */
	InterpCapsuleHalfHeight(DeltaTime);
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
			const auto* TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			if (TraceHitWeapon) {
				if (HightlightSlot == -1) {
					/** Not currently highlight a slot; highlight one */
					HighlightInventorySlot();
				}
			}
			else {
				/** is a slot being highlight? */
				if (HightlightSlot != -1) {
					/** Stop highlighting */
					UnhighlightInventorySlot();
				}
			}

			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EQUIPINTERPING) {
				TraceHitItem = nullptr;
			}
			
			if (TraceHitItem && TraceHitItem->GetPickupWidget()) {
				/** Show Item's pickup widget  */
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();

				if (Inventory.Num() >= INVENTORY_MAX_SIZE) {
					/** Inventory is full */
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else {
				    /** Inventory has room */
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}
			/** We hit an AItem last frame */
			if (TraceHitItemLastFrame) {
				if (TraceHitItemLastFrame != TraceHitItem) {
					/** We are hitting a different aitem this frame from last frame */
					/** Or AItem is null */
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
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
		TraceHitItemLastFrame->DisableCustomDepth();
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

void ANiceCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip) {
		/** Get the hand socket */
		const USkeletalMeshSocket* RightHandSocket{ GetMesh()->GetSocketByName("right_hand_socket") };
		if (RightHandSocket) {
			/** Attach the weapon to the hand socket */
			RightHandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		if (EquippedWeapon == nullptr) {
			/** -1 == no equippedWeapon yet, No need to reverse the icon animation */
			OnEquipItem.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else if (!bSwapping) {
			OnEquipItem.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
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
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (TraceHitItem) {
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr;
	}
}

void ANiceCharacter::SelectButtonReleased()
{

}

void ANiceCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex()) {
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}
	DropWeapon();
	EquipWeapon(WeaponToSwap, true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void ANiceCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_Ar, StartingARAmmo);
}

bool ANiceCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;
	return EquippedWeapon->GetAmmo() > 0;
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
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ANiceCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &ANiceCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &ANiceCharacter::FireButtonReleased);
	
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &ANiceCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &ANiceCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &ANiceCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &ANiceCharacter::SelectButtonReleased);
	
	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &ANiceCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ANiceCharacter::CrouchButtonPressed);
	
	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &ANiceCharacter::FKeyPressed);
	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &ANiceCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &ANiceCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &ANiceCharacter::ThreeKeyPressed);
	PlayerInputComponent->BindAction("4Key", IE_Pressed, this, &ANiceCharacter::FourKeyPressed);
	PlayerInputComponent->BindAction("5Key", IE_Pressed, this, &ANiceCharacter::FiveKeyPressed);

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

void ANiceCharacter::Jump()
{
	if (bCrouching) {
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else{
		Super::Jump();
	}
}

void ANiceCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}
void ANiceCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

void ANiceCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching)
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	else
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;

	const float InterpHalfHeight = FMath::FInterpTo(
		GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.0f);

	/** Negative value if crouching; Positive value id standing */
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset{ 0.0f, 0.0f, -DeltaCapsuleHalfHeight };

	GetMesh()->AddLocalOffset(MeshOffset);
	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void ANiceCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchingMovementSpeed;
}

void ANiceCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching) GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
}

void ANiceCharacter::PickupAmmo(AAmmo* Ammo)
{
	/** Check to see if ammomap contains ammo's ammotype */
	if (AmmoMap.Find(Ammo->GetAmmoType())) {
		/** Get amount of ammo in our ammomap for ammo's type */
		int32 ammoCount{ AmmoMap[Ammo->GetAmmoType()] };
		ammoCount += Ammo->GetItemCount();
		/** Set the amount of ammo in the map for this type  */
		AmmoMap[Ammo->GetAmmoType()] = ammoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType()) {

		/** check to see if the gun is empty */
		if (EquippedWeapon->GetAmmo() == 0) {
			ReloadWeapon();
		}
	}
	Ammo->Destroy();
}

void ANiceCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	if (WeaponHasAmmo()) {
		PlayFireSound();
		SendBullet();
		PlayGunFireMontage();
		StartCrosshairBulletFire();
		EquippedWeapon->DecreamentAmmo();

		StartFireTimer();

		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol) {
			/** Start moving slide timer */
			EquippedWeapon->StartSlideTimer();
		}
	}
}

void ANiceCharacter::PlayFireSound()
{
	/** Play Fire Sound */
	if (EquippedWeapon->GetFireSound()) {
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void ANiceCharacter::SendBullet()
{
	/** Send Bullet */
	const USkeletalMeshSocket *BarrelSocket{
		EquippedWeapon->GetItemMesh()->GetSocketByName("barrelSocket")};
		if (BarrelSocket) {
			const FTransform SocketTransform{ BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh()) };
			if (EquippedWeapon->GetMuzzleFlash()) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), 
					SocketTransform);
			}
			
			FVector BeamEnd;
			bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
			if (bBeamEnd) {
			if (ImapctParticles) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImapctParticles, BeamEnd);
			}
			
			UParticleSystemComponent *Beam{UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform)};
			if (Beam) {
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

void ANiceCharacter::PlayGunFireMontage()
{
	/** Play hit fire montage */
	auto AnimInstance{GetMesh()->GetAnimInstance()};
	if (AnimInstance && HipFireMontage) {
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"), HipFireMontage);
	}
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
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading && 
		CombatState != ECombatState::ECS_Equipping) {
		Aim();
	}
}

void ANiceCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
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
	FireWeapon();
}

void ANiceCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void ANiceCharacter::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer, this, 
		&ANiceCharacter::AutoFireReset, EquippedWeapon->GetAutoRateOfFire());
}

void ANiceCharacter::AutoFireReset()
{
	SetUnoccupied();
	if (EquippedWeapon == nullptr) return;
	if (WeaponHasAmmo()) {
		if (bFireButtonPressed && EquippedWeapon->GetAutomatic()) {
			FireWeapon();
		}
	}
	else {
		ReloadWeapon();
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

/** No longer needed; AItem has GetInterpLocation */
// FVector ANiceCharacter::GetCameraInterpolatedLocation()
// {
// 	const FVector CameraWorldLocation{ GetFollowCamera()->GetComponentLocation() };
// 	const FVector CameraForwards{ GetFollowCamera()->GetForwardVector() };
// 	//	Desired = CameraWorldLocation + Forward * A + UP * B
// 	return CameraWorldLocation + CameraForwards * CameraInterpDistance +
// 		FVector(0.0f, 0.0f, CameraInterpElevation);
// }

void ANiceCharacter::GetPickupItem(AItem* Item)
{
	Item->PlayEquipSound();
	if (auto Weapon = Cast<AWeapon>(Item)) {
		if (Inventory.Num() < INVENTORY_MAX_SIZE) {
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PICKEDUP);
		}
		else {	/** Inventory is full; swap with equipped weapon */
			SwapWeapon(Weapon);
		}
	}
	if (auto ammo = Cast<AAmmo>(Item)) {
		PickupAmmo(ammo);
	}
}

void ANiceCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void ANiceCharacter::ReloadWeapon() 
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;
	
	//	do we have ammo of the correct type?
	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull()) {
		
		if (bAiming) StopAiming();
		CombatState = ECombatState::ECS_Reloading;
		auto* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadingMontage) {
			AnimInstance->Montage_Play(ReloadingMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

void ANiceCharacter::FinishReloading()
{
	/** update the combat state */
	//CombatState = ECombatState::ECS_Unoccupied;
	SetUnoccupied();
	if (bAimingButtonPressed) {
		Aim();
	}

	if (EquippedWeapon == nullptr) return;
	const auto ammoType{ EquippedWeapon->GetAmmoType() };
	/** update the ammomap */
	if (AmmoMap.Contains(ammoType)) {
		/** amount of ammo the Character is carrying of the Equippedweapon type */
		int32 CarriedAmmo = AmmoMap[ammoType];
		/** space left in the magzine of Equipped weapon */
		const int32 magEmptySpace = EquippedWeapon->GetMagazineSize() - EquippedWeapon->GetAmmo();
		
		if (magEmptySpace > CarriedAmmo) {
			/**  reload the magzine with all the ammo we have */
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(ammoType, CarriedAmmo);
		}
		else {
			/** fill the magzine */
			EquippedWeapon->ReloadAmmo(magEmptySpace);
			CarriedAmmo -= magEmptySpace;
			AmmoMap.Add(ammoType, CarriedAmmo);
		}
	}
}

void ANiceCharacter::GrabClip()
{
	//show("GrabClip from NiceCharacter");	//!	Debug
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	/** index for the clip bone on the Equipped weapon */
	int32 clipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(
		EquippedWeapon->GetClipBoneName()) };
	/** Store the transform of the clip */
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(clipBoneIndex);
	
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransform);
	
	EquippedWeapon->SetMovingClip(true);
}

void ANiceCharacter::ReleaseClip()
{
	//show("ReleaseClip from NiceCharacter");	//!	Debug
	EquippedWeapon->SetMovingClip(false);
}

bool ANiceCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;
	auto AmmoType = EquippedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType)) {
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void ANiceCharacter::CrouchButtonPressed() 
{
	if (!GetCharacterMovement()->IsFalling()) {
		bCrouching = !bCrouching;
	}
	if (bCrouching) {
		GetCharacterMovement()->MaxWalkSpeed = CrouchingMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void ANiceCharacter::InitializeInterpLocations()
{
	FInterpLocation weaponLocation{ WeaponInterpComp, 0 };
	InterpLocations.Add(weaponLocation);

	FInterpLocation inpterpLocation1{ InterpComp1, 0 };
	InterpLocations.Add(inpterpLocation1);

	FInterpLocation interpLocation2{ InterpComp2, 0 };
	InterpLocations.Add(interpLocation2);

	FInterpLocation interpLocation3{ InterpComp3, 0 };
	InterpLocations.Add(interpLocation3);

	FInterpLocation interpLocation4{ InterpComp4, 0 };
	InterpLocations.Add(interpLocation4);

	FInterpLocation interpLocation5{ InterpComp5, 0 };
	InterpLocations.Add(interpLocation5);

	FInterpLocation interpLocation6{ InterpComp6, 0 };
	InterpLocations.Add(interpLocation6);

}

void ANiceCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void ANiceCharacter::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void ANiceCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void ANiceCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void ANiceCharacter::FourKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void ANiceCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

void ANiceCharacter::ExchangeInventoryItems(int32 CurrentItem, int32 NewItemIndex)
{
	if ((CurrentItem != NewItemIndex) && (NewItemIndex < Inventory.Num()) && 
		(CombatState == ECombatState::ECS_Unoccupied || CombatState == ECombatState::ECS_Equipping)) 
	{
		if (bAiming) StopAiming();

		auto OldEquippedItem = EquippedWeapon;
		auto NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewWeapon);
	
		OldEquippedItem->SetItemState(EItemState::EIS_PICKEDUP);
		NewWeapon->SetItemState(EItemState::EIS_EQUIPPED);
	
		CombatState = ECombatState::ECS_Equipping;
		if (auto* AnimInstance = GetMesh()->GetAnimInstance()) {
			if (EquipMontage) {
				AnimInstance->Montage_Play(EquipMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(FName("Equip"));
			}
		}
		NewWeapon->PlayEquipSound(true);
	}
}

int32 ANiceCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++) {
		if (Inventory[i] == nullptr) {
			return i;
		}
	}
	if (Inventory.Num() < INVENTORY_MAX_SIZE) {
		return Inventory.Num();
	}
	return -1;	//* Inventory is full
}

void ANiceCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot{ GetEmptyInventorySlot() };
	OnHighlightSlot.Broadcast(EmptySlot, true);
	HightlightSlot = EmptySlot;
}

void ANiceCharacter::UnhighlightInventorySlot()
{
	OnHighlightSlot.Broadcast(HightlightSlot, false);
	HightlightSlot = -1;
}

EPhysicalSurface ANiceCharacter::GetSurfaceType()
{
	FHitResult hitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.f, 0.f, -400.0f) };
	FCollisionQueryParams QuaryParams;
	QuaryParams.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(hitResult, Start, End, ECollisionChannel::ECC_Visibility, 
		QuaryParams);
	return UPhysicalMaterial::DetermineSurfaceType(hitResult.PhysMaterial.Get());
}
 
int32 ANiceCharacter::GetInterpLocationIndex()
{
	// Initialize the lowest index to 1
	int32 LowestIndex = 1;
	// Initialize the lowest count to the maximum integer value
	int32 lowestCount = INT_MAX;
	// Loop through the InterpLocations array
	for (int32 i = 1; i < InterpLocations.Num(); i++) {
		// If the current item count is less than the lowest count
		if (InterpLocations[i].ItemCount < lowestCount) {
			LowestIndex = i;
			lowestCount = InterpLocations[i].ItemCount;
		}
	}
	return LowestIndex;
}

FInterpLocation ANiceCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num()) {
		return InterpLocations[Index];
	}
	return FInterpLocation();
}

void ANiceCharacter::IncrementInterpLocationItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1) return;
	if (InterpLocations.Num() >= Index) {
		InterpLocations[Index].ItemCount += Amount;
	}
}

void ANiceCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, 
		&ANiceCharacter::ResetPickupSoundTimer, PickupSoundResetTime);
}

void ANiceCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this,
		&ANiceCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

void ANiceCharacter::SetUnoccupied() 
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void ANiceCharacter::ResetAiming()
{
	if (bAimingButtonPressed) {
		Aim();
	}
}