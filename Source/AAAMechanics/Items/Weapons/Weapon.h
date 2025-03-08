// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..//Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class AAAMECHANICS_API AWeapon : public AItem
{
	GENERATED_BODY()

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	/** Ammo count for this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;
	
public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	
	/** adds the impluse to the weapon */
	void ThrowWeapon();
	/**  */
	void DecreamentAmmo();
	
	/*************	 GETTTERS	***************/
	FORCEINLINE int32 GetAmmo() const { return Ammo; }

protected:
	void StopFalling();

};
