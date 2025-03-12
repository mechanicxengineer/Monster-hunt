// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..//Item.h"
#include "..//..//AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun 	UMETA(DisplayName="Submachine Gun"),
	EWT_AssaultRifle 	UMETA(DisplayName="Assault Rifle"),

	EWT_Max				UMETA(DisplayName="DefaultMax")
};

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
	
	/** maximum ammo that our weapon can hold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MagazineSize;

	/** The type of weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	
	/** the type of ammo for this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;
	/** FName for the reload Montage section */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

	/** true when moveing the clip while reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bMovingClip;
	
	/** Name for the clip bone */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ClipBoneName;

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	
	/** adds the impluse to the weapon */
	void ThrowWeapon();
	/** called from character class when firing weapon */
	void DecreamentAmmo();
	void ReloadAmmo(int32 Amount);
	
	/*************	 GETTERS	***************/
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE int32 GetMagazineSize() const { return MagazineSize; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	
	/*************	 SETTERS	***************/
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

protected:
	void StopFalling();

};
