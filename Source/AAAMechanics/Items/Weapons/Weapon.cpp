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
    ClipBoneName(TEXT("smg_clip")),
    SlideDisplacement(0.0f),
    SlideDisplacementTime(0.2f),
    bMovingSlide(false),
    MaxSlideDisplacement(4.0f),
    MaxRecoilRotation(20.0f),
    bAutomatic(true)
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
    /** update slide on pistol */
    UpdateSlideDisplacement();
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

    EnableGlowMaterial();

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

bool AWeapon::ClipIsFull()
{
    return Ammo >= MagazineSize;
}

void AWeapon::StartSlideTimer()
{
    bMovingSlide = true;
    GetWorldTimerManager().SetTimer(SlideTimer, this, 
        &AWeapon::FinishMovingSlide, SlideDisplacementTime);
}

void AWeapon::OnConstruction(const FTransform& Transform) 
{
    Super::OnConstruction(Transform);
    // update this line with newer one
    const FString WeaponTablePath(TEXT("DataTable'/Game/_Game/DataTables/DT_Weapon.DT_Weapon'"));
    UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(),
        nullptr, *WeaponTablePath));
    if (WeaponTableObject) {
        FWeaponDataTable* WeaponDataRow = nullptr;
        switch (WeaponType)
        {
            case EWeaponType::EWT_SubmachineGun:
                WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubmachineGun"), TEXT(""));
                break;
            case EWeaponType::EWT_AssaultRifle:
                WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
                break;
                case EWeaponType::EWT_Pistol:
                WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));
                break;
        }

        if (WeaponDataRow) {
            AmmoType = WeaponDataRow->AmmoType;
            Ammo = WeaponDataRow->WeaponAmmo;
            MagazineSize = WeaponDataRow->MagazineSize;
            SetPickupSound(WeaponDataRow->PickupSound);
            SetEquipSound(WeaponDataRow->EquipSound);
            GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
            SetItemName(WeaponDataRow->ItemName);
            SetIconItem(WeaponDataRow->InventoryIcon);
            SetAmmoIcon(WeaponDataRow->AmmoIcon);

            SetMaterialInstance(WeaponDataRow->MaterialInstance);
            PreviousMaterialIndex = GetMaterialIndex();
            GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
            SetMaterialIndex(WeaponDataRow->MaterialIndex);
            SetClipBoneName(WeaponDataRow->ClipBoneName);
            SetReloadMontageSection(WeaponDataRow->ReloadMontageSection);
            GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimationBP);

            CrosshairsMiddle = WeaponDataRow->CrosshairsMiddle;
            CrosshairsLeft = WeaponDataRow->CrosshairsLeft;
            CrosshairsTop = WeaponDataRow->CrosshairsTop;
            CrosshairsRight = WeaponDataRow->CrosshairsRight;
            CrosshairsBottom = WeaponDataRow->CrosshairsBottom;
            AutoRateOfFire = WeaponDataRow->AutoRateOfFire;
            MuzzleFlash = WeaponDataRow->MuzzleFlash;
            FireSound = WeaponDataRow->FireSound;
            BoneToHide = WeaponDataRow->BoneToHide;
            bAutomatic = WeaponDataRow->bAutomatic;
            Damage = WeaponDataRow->Damage;
            HeadShotDamage = WeaponDataRow->HeadshotDamage;
        }

        if (GetMaterialInstance()) {
            SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
            GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColor"), GetGlowColor());
            GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());
    
            EnableGlowMaterial();
        }
    }
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    if (BoneToHide != FName("")) {
        GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
    }
}

void AWeapon::StopFalling()
{
    bFalling = false;
    SetItemState(EItemState::EIS_PICKUP);
    StartPulseTimer();
}

void AWeapon::UpdateSlideDisplacement()
{
    if (SlideDisplacementCurve && bMovingSlide) {
        const float ElapsedTime{ GetWorldTimerManager().GetTimerElapsed(SlideTimer) };
        const float CurveValue{ SlideDisplacementCurve->GetFloatValue(ElapsedTime) };
        SlideDisplacement = CurveValue * MaxSlideDisplacement;
        RecoilRotation = CurveValue * MaxRecoilRotation;
    }
}

void AWeapon::FinishMovingSlide()
{
    bMovingSlide = false;
}
