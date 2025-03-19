// Fill out your copyright notice in the Description page of Project Settings.

//	engine header
#include "Item.h"
#include "Components//BoxComponent.h"
#include "Components//SphereComponent.h"
#include "Components//SkeletalMeshComponent.h"
#include "Components//WidgetComponent.h"
#include "Components//SceneComponent.h"

#include "Kismet//GameplayStatics.h"
#include "Sound//SoundCue.h"

#include "Camera//CameraComponent.h"
//	custom header
#include "..//Characters//NiceCharacter.h"


AItem::AItem() :
	ItemName("Default"),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_COMMON),
	ItemState(EItemState::EIS_PICKUP),
	/** Item interp variables */
	ItemInterpStartLocation(FVector(0.0f)),
	CameraTargetLocation(FVector(0.0f)),
	bInterping(false),
	ZCurveTime(0.7f),
	ItemInterpX(0.0f),
	ItemInterpY(0.0f),
	InterpInitialYawOffset(0.0f),
	ItemType(EItemType::EIT_MAX),
	InterpLocIndex(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ItemMesh");
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(GetRootComponent());
	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidget->SetDrawSize(FVector2D(300.0f, 120.0f));

	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(GetRootComponent());
	AreaSphere->SetSphereRadius(200.0f);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	/* Hide Pickup Widget */
	if (PickupWidget) {
		PickupWidget->SetVisibility(false);
	}
	/** Set Active Stars Array based on Item Rarity */
	SetActiveStars();

	/** Setup overlap for Areasphere */
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);

	/** Set Item Properties based on Item State */
	SetItemProperties(ItemState);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/** handle item interping when in the equipInterping state */
	ItemInterp(DeltaTime);
}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr) {
		if (auto niceCharacter = Cast<ANiceCharacter>(OtherActor)) {
			niceCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	 int32 OtherBodyIndex)
{
	if (OtherActor != nullptr) {
		if (auto niceCharacter = Cast<ANiceCharacter>(OtherActor)) {
			niceCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

void AItem::SetActiveStars()
{
	// The 0 element isn't used
	for (int32 i = 0; i <= 5; i++) {
		ActiveStars.Add(false);
	}

	switch (ItemRarity) {
		case EItemRarity::EIR_DAMAGED:
			ActiveStars[1] = true;
			break;
		case EItemRarity::EIR_COMMON:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			break;
		case EItemRarity::EIR_UNCOMMON:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			break;
		case EItemRarity::EIR_RARE:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			ActiveStars[4] = true;
			break;
		case EItemRarity::EIR_LEGENDARY:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			ActiveStars[4] = true;
			ActiveStars[5] = true;
			break;
		default:
			break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State) {
		case EItemState::EIS_PICKUP:
			/** Set mesh and area sphere properties */
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			break;
		case EItemState::EIS_EQUIPPED:
			PickupWidget->SetVisibility(false);
			/** Set mesh and area sphere properties */
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;
		case EItemState::EIS_FALLING:
			/** Set mesh and area sphere properties */
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ItemMesh->SetSimulatePhysics(true);
			ItemMesh->SetEnableGravity(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;
		case EItemState::EIS_EQUIPINTERPING:
			PickupWidget->SetVisibility(false);
			/** Set mesh and area sphere properties */
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			/** Set areasphere properties */
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			/** Set collision box properties */
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			break;
		default:
			break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character) {
		/** subtrust 1 from the count of the interp location struct */
		Character->IncrementInterpLocationItemCount(InterpLocIndex, -1);
		Character->GetPickupItem(this);
	}
	/** Set scale back to normal */
	SetActorScale3D(FVector(1.0f));
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) { return; }

	if (Character && ItemZCurve) {
		/** Elapsed time since we started ItemInterpTimer  */
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		/** Get curve value corresponding to Elapsed time */
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		/** Get the item's initial location when the curve started */
		FVector ItemLocation = ItemInterpStartLocation;
		const FVector CameraInterpLocation{ GetInterpLocation() };

		/** Vector from item to camera inter location, x and y are zeroed out */
		const FVector ItemToCamera{ FVector(0.0f, 0.0f,(CameraInterpLocation - ItemLocation).Z) };
		/** Scale factor to multiple with CurveValue */
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{ GetActorLocation() };
		/** Interpolated X value */
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime,
			30.0f);
		/** Interpolated Y value */
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime,
			30.0f);

		/** Set X and Y of ItemLocation to Interped values */
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		/** Adding curve value to the Z Component of the initial Location ( scale by DeltaZ ) */
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, 
			ETeleportType::TeleportPhysics);
		
		/** Camera rotation this frame */
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		/** Camera rotation plus inital yaw offset */
		FRotator ItemRotation{ 0.0f, CameraRotation.Yaw + InterpInitialYawOffset, 0.0f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);
		
		if (ItemScaleCurve) {
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) return FVector::ZeroVector;

	switch (ItemType) {
	case EItemType::EIT_AMMO:
		return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;
		case EItemType::EIT_WEAPON:
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
		break;
	default:
		break;
	}
	return FVector();
}

void AItem::PlayPickupSound()
{
	if (Character) {
		if (Character->ShouldPlayPickupSound()) {
			Character->StartPickupSoundTimer();
			if (PickupSound) {
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::PlayEquipSound()
{
	if (Character) {
		if (Character->ShouldPlayEquipSound()) {
			Character->StartEquipSoundTimer();
			if (EquipSound) {
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(ANiceCharacter* Char)
{
	Character = Char;									/** Store a handle to the character */
	ItemInterpStartLocation = GetActorLocation();		/** store initial location of the item */
	bInterping = true;									/** Set interping to true */
	SetItemState(EItemState::EIS_EQUIPINTERPING);		/** Set item state to interping */

	PlayPickupSound();

	/** Get array in Interplocations with the lowest item count */
	InterpLocIndex = Character->GetInterpLocationIndex();
	/** add 1 to the item count for this interp location struct */
	Character->IncrementInterpLocationItemCount(InterpLocIndex, 1);

	GetWorldTimerManager().SetTimer(ItemInterpTimer, this,
		&AItem::FinishInterping, ZCurveTime);
	
	/** Get initial yaw of the camera */
	const float CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
	/** Get Initial yaw of the Item */
	const float ItemRotationYaw{ GetActorRotation().Yaw };

	/** Initial yaw offset between camera and item */
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

}

