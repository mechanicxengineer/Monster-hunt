// Fill out your copyright notice in the Description page of Project Settings.

//  Engine includes
#include "NiceAnimInstance.h"
#include "NiceCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

//  Custom includes
#include "..//DebugMacros.h"
#include "..//Common//AdvLog.h"

UNiceAnimInstance::UNiceAnimInstance() :
    Speed(0.0f),
    bIsInAir(false),
    bIsAccelerating(false),
    MovementOffsetYaw(0.0f),
    LastMovementOffsetYaw(0.0f),
    bAiming(false),
    TIP_CharacterYaw(0.0f),
    TIP_CharacterYawLastFrame(0.0f),
    RootYawOffset(0.0f),
    Pitch(0.0f),
    bReloading(false),
    OffsetState(EOffsetState::EOS_Hip),
    CharacterRotation(FRotator::ZeroRotator),
    CharacterRotationLastFrame(FRotator::ZeroRotator),
    YawDelta(0.0f),
    RecoilWeight(1.0f),
    bTurningInPlace(false)
{}

void UNiceAnimInstance::NativeInitializeAnimation()
{
    NiceCharacter = Cast<ANiceCharacter>(TryGetPawnOwner());
}

void UNiceAnimInstance::UpdateAnimationProperties(float DeltaSeconds)
{
    if (NiceCharacter == nullptr) {
        NiceCharacter = Cast<ANiceCharacter>(TryGetPawnOwner());
    }
    if (NiceCharacter) {
        bCrouching = NiceCharacter->GetCrouching();
        bReloading = NiceCharacter->GetCombatState() == ECombatState::ECS_Reloading;

        /** Get the speed of the character form velocity */
        FVector Velocity{ NiceCharacter->GetVelocity() };
        Velocity.Z = 0;
        Speed = Velocity.Size();

        /** Is the character in the air ? */
        bIsInAir = NiceCharacter->GetCharacterMovement()->IsFalling();
        /** Is the character accelerating ? */
        bIsAccelerating = NiceCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;

        FRotator AimRotation = NiceCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(NiceCharacter->GetVelocity());
        MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
        if (NiceCharacter->GetVelocity().Size() > 0) {
            LastMovementOffsetYaw = MovementOffsetYaw;
        }
        //SHOW("Base aim rotation: {}", Rotation.Yaw);              //! Debug
        //SHOW("Movement Rotation: {}", MovementRotation.Yaw);      //! Debug
        //SHOW("Movement Offset Yaw: {}", MovementOffsetYaw);       //! Debug
        bAiming = NiceCharacter->GetAiming();

        if (bReloading) {
            OffsetState = EOffsetState::EOS_Reloading;
        }
        else if (bIsInAir) {
            OffsetState = EOffsetState::EOS_InAir;
        }
        else if (NiceCharacter->GetAiming()) {
            OffsetState = EOffsetState::EOS_Aiming;
        }
        else {
            OffsetState = EOffsetState::EOS_Hip;
        }
    }
    TurnInPlace();
    Lean(DeltaSeconds);
}

void UNiceAnimInstance::TurnInPlace()
{
    if (NiceCharacter == nullptr) return;
    Pitch = NiceCharacter->GetBaseAimRotation().Pitch;

    if (Speed > 0.0f|| bIsInAir) {
        /** Don't want to turn in plaace, Character is moving */
        RootYawOffset = 0.0f;
        TIP_CharacterYaw = NiceCharacter->GetActorRotation().Yaw;
        TIP_CharacterYawLastFrame = TIP_CharacterYaw;
        RotationCurveLastFrame = 0.0f;
        RotationCurve = 0.0f;
    }
    else {
        TIP_CharacterYawLastFrame = TIP_CharacterYaw;
        TIP_CharacterYaw = NiceCharacter->GetActorRotation().Yaw;
        const float TIP_YawDelta { TIP_CharacterYaw - TIP_CharacterYawLastFrame };

        /** root yaw offset, update and clamped to [-180, 180] */
        RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIP_YawDelta);

        /** 1.0 if turning, 0.0 if not */
        const float Turning{ GetCurveValue(TEXT("Turning")) };
        if (Turning > 0) {
            bTurningInPlace = true;
            RotationCurveLastFrame = RotationCurve;
            RotationCurve = GetCurveValue(TEXT("Rotation"));
            const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

            /** RootYawOffset > 0, then Turning left. RootyawOffset < 0, then Turning Right  */
            RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

            const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
            if (ABSRootYawOffset > 90.0f) {
                const float YawExcess{ ABSRootYawOffset - 90.0f };
                RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
                
            }
        }
        else {
            bTurningInPlace = false;
        }
        //showargplus("RootYawOffset: %f", RootYawOffset);
    }
    /** Set the recoil */
    if (bTurningInPlace) {
        if (bReloading)
            RecoilWeight = 1.0f;
        else
            RecoilWeight = 0.0f;
    }
    else {  /** not turning in place */
        if (bCrouching) {
            if (bReloading)
                RecoilWeight = 1.0f;
            else
                RecoilWeight = 0.1f;
        }
        else {
            if (bAiming || bReloading)
                RecoilWeight = 1.0f;
            else
                RecoilWeight = 0.5f;
        }
    }
}

void UNiceAnimInstance::Lean(float detlaTime)
{
    if (NiceCharacter == nullptr) return;
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = NiceCharacter->GetActorRotation();

    const FRotator Delta{ 
        UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };
    const float Target{ Delta.Yaw / detlaTime };
    const float Interp{ FMath::FInterpTo(YawDelta, Target, detlaTime, 6.0f) };
    YawDelta = FMath::Clamp(Interp, -90.0f, 90.0f);

    //showargplus("YawDelta: %f", Delta.Yaw);           //! Debug
}