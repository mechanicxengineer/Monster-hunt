// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine//Datatable.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_DAMAGED 	UMETA(DisplayName = "Damaged"),
	EIR_COMMON 		UMETA(DisplayName = "Common"),
	EIR_UNCOMMON 	UMETA(DisplayName = "Uncommon"),
	EIR_RARE 		UMETA(DisplayName = "Rare"),
	EIR_LEGENDARY 	UMETA(DisplayName = "Legendary"),

	EIR_MAX 		UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_PICKUP				UMETA(DisplayName = "Pickup"),
	EIS_EQUIPINTERPING		UMETA(DisplayName = "EquipInterping"),
	EIS_EQUIPPED			UMETA(DisplayName = "Equipped"),
	EIS_PICKEDUP			UMETA(DisplayName = "PickedUp"),
	EIS_FALLING				UMETA(DisplayName = "Falling"),
	
	EIS_MAX 				UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_AMMO				UMETA(DisplayName = "Ammo"),
	EIT_WEAPON				UMETA(DisplayName = "Weapon"),

	EIT_MAX 				UMETA(DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStars;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackground;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;

};

UCLASS()
class AAAMECHANICS_API AItem : public AActor
{
	GENERATED_BODY()
	
	/** Skeletal mesh for the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ItemMesh;
	
	/** Line trace colliders with box to show HUD widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	/** Popup widget for when the player looks at the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickupWidget;

	/** Enables item tracing when overlapped */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	/** The name which appers on the Pickup Widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	/** Item Count (ammo, health, etc) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;
	
	/** Item rarity - determines number of stars in Pickup widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;
	
	/** Array of stars to show on the Pickup widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;
	
	/** State of the Item */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;
	
	/** The curve asset to use for the item's Z location when interping */
	UPROPERTY(EditdefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemZCurve;
	
	/** Starting location when interping begins */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;
	/** Target interp location in fornt of the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation;
	/** true when interping */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bInterping;
	
	/** Plays when we start interping */
	FTimerHandle ItemInterpTimer;
	/** Durtion of the curve and time */
	UPROPERTY(EditdefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime;
	
	/** Pointer to the Character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class ANiceCharacter* Character;
	/** X and Y for the item while interping in the equipInterping state */
	float ItemInterpX;
	float ItemInterpY;
	
	/** Initial yaw offset between the camera and the interping item */
	float InterpInitialYawOffset;
	/** Curve used to scale the item when interping */
	UPROPERTY(EditdefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;
	
	/** Sound played when item is picked up */
	UPROPERTY(EditdefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* PickupSound;
	
	/** Sound played when the item is equipped */
	UPROPERTY(EditdefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;
	
	/** Enum for the type of item this Item is */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;
	
	/** Index of interp location this is interping to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 InterpLocIndex;
	
	/** Index for the material we would like to change at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;
	
	/** Dynamic instance that we can change at runtime */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	
	/** Material instance used with the dynamic material */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;
	
	bool bCanChangeCustomDepth;
	
	/** Curve to drive the dynamic meterial parameters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveVector* PulseCurve;
	/** Curve to drive the dynamic meterial parameters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveVector* InterpPulseCurve;
	/**  */
	FTimerHandle PulseTimer;
	/** time for the pulse timer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float PulseCurveTime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float GlowAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float FresnelExponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float FresnelReflectFraction;
	
	/** Icon Item for this item in the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconItem;

	/** Ammo Item for this item in the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoItem;
	
	/** Slot in the Inventory array */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex;
	
	/** true when the character is full */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	bool bCharacterInventoryFull;
	
	/** ITem rarity data table */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	class UDataTable* ItemRarityDataTable;
	
	/** Color in the glow material */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	FLinearColor GlowColor;
	/** light color in the pickup widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	FLinearColor LightColor;
	/** dark color in the pickup widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	FLinearColor DarkColor;
	/** Number of stars in the pickup widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	int32 NumberOfStars;
	/** Background icon for the inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconBackground;

public:	
	// Sets default values for this actor's properties
	AItem();
	
protected:
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** Called when begin overlapping Areasphere */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** Called when end overlapping AreaSphere */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex);
		
	/** Set the active stars array of bools based on rarity */
	void SetActiveStars();
	/** Set the item properties (name, mesh, etc) based on the ItemState */
	virtual void SetItemProperties(EItemState State);
	
	/** Called when iteminterping is finished */
	void FinishInterping();
	
	/** Handles item interpolation when inerping is true */
	void ItemInterp(float DeltaTime);
	
	/** Get interp location based on the item type */
	FVector GetInterpLocation();
	
	void PlayPickupSound(bool bForcePlaySound = false);
	virtual void InitializeCustomDepth();
	
	void StartPulseTimer();
	void ResetPulseTimer();
	void UpdatePulse();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	/*************	 GETTTERS	***************/
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	FORCEINLINE int32 GetSlotIndex() { return SlotIndex; }
	
	void PlayEquipSound(bool bForcePlaySound = false);
	
	/*************	 SETTERS	***************/
	void SetItemState(EItemState State);
	void SetSlotIndex(int32 Index) { SlotIndex = Index; }
	void SetCharacter(ANiceCharacter* Char) { Character = Char; }
	void SetCharacterInventoryFull(bool bFull) { bCharacterInventoryFull = bFull; }
	/******************* ***********************/
	
	/** Called from the Nicecharacter class */
	void StartItemCurve(ANiceCharacter* Char, bool bForcePlaySound = false);
	
	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
	
	void EnableGlowMaterial();
	void DisableGlowMaterial();
	
	
};
