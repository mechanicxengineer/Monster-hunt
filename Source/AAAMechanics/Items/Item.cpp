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
#include "Curves//CurveVector.h"
//	custom header
#include "..//Characters//NiceCharacter.h"
#include "..//DebugMacros.h"


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
	InterpLocIndex(0),
	MaterialIndex(0),
	bCanChangeCustomDepth(true),
	/** Dynamic meterial parameters */
	PulseCurveTime(5.0f),
	GlowAmount(150.0f),
	FresnelExponent(3.0f),
	FresnelReflectFraction(4.0f),
	SlotIndex(0),
	bCharacterInventoryFull(false)
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

void AItem::OnConstruction(const FTransform& Transform)
{
	/** load the data in the item rarity data table */
	/** path the the item data table */
	FString RarityDataPath(TEXT("DataTable'/Game/_Game/DataTables/DT_ItemRarity.DT_ItemRarity'"));
	UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityDataPath));
	if (RarityTableObject) {
		FItemRarityTable* RarityRow = nullptr;
		switch (ItemRarity) {
			case EItemRarity::EIR_DAMAGED:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Damaged"), TEXT(""));
				break;
			case EItemRarity::EIR_COMMON:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
				break;
			case EItemRarity::EIR_UNCOMMON:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Uncommon"), TEXT(""));
				break;
			case EItemRarity::EIR_RARE:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
				break;
			case EItemRarity::EIR_LEGENDARY:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
				break;
		}
		
		if (RarityRow) {
			GlowColor = RarityRow->GlowColor;
			LightColor = RarityRow->LightColor;
			DarkColor = RarityRow->DarkColor;
			NumberOfStars = RarityRow->NumberOfStars;
			IconBackground = RarityRow->IconBackground;
			if (GetItemMesh()) {
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
			}
		}
	}

	if (MaterialInstance) {
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColor"), GlowColor);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);

		EnableGlowMaterial();
	}
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

	/** set custom depth to disabled */
	InitializeCustomDepth();

	StartPulseTimer();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/** handle item interping when in the equipInterping state */
	ItemInterp(DeltaTime);
	/** Get curve values from pulsecurve and dynamic meterial parameters */
	UpdatePulse();
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
			niceCharacter->UnhighlightInventorySlot();
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
			ItemMesh->SetVisibility(true);
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
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			/** Set collision box properties */
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
			break;
		case EItemState::EIS_PICKEDUP:
			PickupWidget->SetVisibility(false);
			/** Set mesh and area sphere properties */
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(false);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			/** Set areasphere properties */
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			/** Set collision box properties */
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		Character->UnhighlightInventorySlot();
	}
	/** Set scale back to normal */
	SetActorScale3D(FVector(1.0f));
	
	DisableGlowMaterial();
	bCanChangeCustomDepth = true;
	DisableCustomDepth();
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
	}
	return FVector();
}
			
void AItem::PlayPickupSound(bool bForcePlaySound)
{
	if (Character) {
		if (bForcePlaySound) {
			if (PickupSound) {
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
		else if (Character->ShouldPlayPickupSound()) {
			Character->StartPickupSoundTimer();
			if (PickupSound) {
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if (Character) {
		if (bForcePlaySound) {
			if (EquipSound) {
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
		else if (Character->ShouldPlayEquipSound()) {
			Character->StartEquipSoundTimer();
			if (EquipSound) {
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

void AItem::EnableCustomDepth()
{
	if (bCanChangeCustomDepth) {
		ItemMesh->SetRenderCustomDepth(true); 
	}
}
void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth) {
		ItemMesh->SetRenderCustomDepth(false);
	}
}
void AItem::InitializeCustomDepth() { DisableCustomDepth(); }

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance) {
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance) {
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}
}

void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_PICKUP) {
		GetWorldTimerManager().SetTimer(PulseTimer, this, 
			&AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::ResetPulseTimer() { StartPulseTimer(); }

void AItem::UpdatePulse()
{
	float ElapsedTime{};
	FVector CurveValue{};
    switch (ItemState)
	{
		case EItemState::EIS_PICKUP:
			if (PulseCurve) {
				ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
				CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
			}
			break;
		case EItemState::EIS_EQUIPINTERPING:
			if (InterpPulseCurve) {
				ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
				CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
			}
			break;
	}

	// Validate that PulseCurve and DynamicMaterialInstance are not null
	if (DynamicMaterialInstance)
	{
		// Safely set material scalar parameters
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);
	}
}


void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(ANiceCharacter* Char, bool bForcePlaySound)
{
	Character = Char;									/** Store a handle to the character */
	ItemInterpStartLocation = GetActorLocation();		/** store initial location of the item */
	bInterping = true;									/** Set interping to true */
	SetItemState(EItemState::EIS_EQUIPINTERPING);		/** Set item state to interping */
	GetWorldTimerManager().ClearTimer(PulseTimer);		/** Clear the pulse timer */
	PlayPickupSound(bForcePlaySound);
	
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
		bCanChangeCustomDepth = false;
	}
	
	
	