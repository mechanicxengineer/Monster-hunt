
#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun 	UMETA(DisplayName="Submachine Gun"),
	EWT_AssaultRifle 	UMETA(DisplayName="Assault Rifle"),
	EWT_Pistol			UMETA(DisplayName="Pistol"),

	EWT_Max				UMETA(DisplayName="DefaultMax")
};
