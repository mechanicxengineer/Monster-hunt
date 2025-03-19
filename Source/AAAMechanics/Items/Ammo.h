// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "..//AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class AAAMECHANICS_API AAmmo : public AItem
{
	GENERATED_BODY()

	/**  Mesh for the ammo pickup */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;
	
	/** Overlap sphere for picking up the ammo */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AmmoCollisionComponent;

	/** Ammo type for ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	/** The texture for the ammo icon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

protected:
	virtual void BeginPlay() override;
	virtual void SetItemProperties(EItemState State) override;

	/** called when Start overlapping areaSphere */
	UFUNCTION()
	void AmmoSphereOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

public:
	AAmmo();
	virtual void Tick(float DeltaTime) override;

	/*************	 GETTTERS	***************/
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() { return AmmoType; }

	/******************************************/
};
