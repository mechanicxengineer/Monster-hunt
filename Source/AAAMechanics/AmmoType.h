
#pragma once


UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName="9mm"),
	EAT_Ar UMETA(DisplayName="Assault Rifle"),

	EAT_Max UMETA(DisplayName="DefaultMax")
};