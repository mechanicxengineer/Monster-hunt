// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon() :
    ThrowWeaponTime(0.7f),
    bFalling(false),
    Ammo(30),
    MagazineSize(30),
    WeaponType(EWeaponType::EWT_SubmachineGun),
    AmmoType(EAmmoType::EAT_9mm),
    ReloadMontageSection(FName(TEXT("reloadSmg"))),
    ClipBoneName(TEXT("smg_clip"))
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /** Keep the Weapon up right */
    if (GetItemState() == EItemState::EIS_FALLING && bFalling) {
        const FRotator MeshRotation{ 0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f };
        GetItemMesh()->SetWorldRotation(MeshRotation, false, 
            nullptr, ETeleportType::TeleportPhysics);
    }
}

void AWeapon::ThrowWeapon()
{
    FRotator MeshRotation{ 0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f };
    GetItemMesh()->SetWorldRotation(MeshRotation, false, 
        nullptr, ETeleportType::TeleportPhysics);

    const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
    const FVector MeshRight{ GetItemMesh()->GetRightVector() };
    /** Direction in which we throw the weapon */
    FVector impluseDirection = MeshRight.RotateAngleAxis(-20.0f, MeshForward);

    float RandomRotation{ 30.0f};
    impluseDirection = impluseDirection.RotateAngleAxis(RandomRotation, FVector(0.0f, 0.0f, 1.0f));
    impluseDirection *= 10'000.0f;
    GetItemMesh()->AddImpulse(impluseDirection);

    bFalling = true;
    GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, 
        &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::DecreamentAmmo()
{
    if (Ammo - 1 <= 0) {
        Ammo = 0;
    }
    else {
        --Ammo;
    }
}

void AWeapon::ReloadAmmo(int32 Amount)
{
    UE_CHECK_F_IMPL(Ammo + Amount <= MagazineSize, 
        TEXT("Attempting to reload with more then magazine size !"));
    Ammo += Amount;
}

void AWeapon::StopFalling()
{
    bFalling = false;
    SetItemState(EItemState::EIS_PICKUP);
}