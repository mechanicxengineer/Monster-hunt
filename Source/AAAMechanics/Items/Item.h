// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
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

public:	
	// Sets default values for this actor's properties
	AItem();

protected:
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
	void SetItemProperties(EItemState State);

	/** Called when iteminterping is finished */
	void FinishInterping();

	/** Handles item interpolation when inerping is true */
	void ItemInterp(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	/*************	 GETTTERS	***************/
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	/*************	 SETTERS	***************/
	void SetItemState(EItemState State);
	/******************* ***********************/
	
	/** Called from the Nicecharacter class */
	void StartItemCurve(ANiceCharacter* Char);
};
