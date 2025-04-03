// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Kismet//GameplayStatics.h"

#include "Sound//SoundCue.h"

#include "Particles//ParticleSystemComponent.h"

#include "Blueprint//UserWidget.h"

#include "..//Common//AdvLog.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f),
	HealthBarDisplayTime(3.f),
	HitReactTimeMin(0.5f),
	HitReactTimeMax(2.0f),
	bCanHitReact(true),
	HitNumberDestroyTime(1.5f)
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateDamageNumbers();
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult _hitResult)
{
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticles,
			_hitResult.Location, FRotator::ZeroRotator, true);
	}
	ShowHealthBar();
	PlayHitMontage(FName("HitFront"));
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f) {
		Health = 0.f;
		Die();
	}
	else {
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarDisplayTimer);
	GetWorldTimerManager().SetTimer(HealthBarDisplayTimer, this, 
		&AEnemy::HideHealthBar, HealthBarDisplayTime);
}

void AEnemy::PlayHitMontage(FName section, float playRate)
{
	if (bCanHitReact) {
		if (auto animInstance = GetMesh()->GetAnimInstance()) {
			animInstance->Montage_Play(HitMontage, playRate);
			animInstance->Montage_JumpToSection(section, HitMontage);
		}
		bCanHitReact = false;
		const float HitReactTime{ FMath::FRandRange(HitReactTimeMin, HitReactTimeMax) };
		GetWorldTimerManager().SetTimer(HitReactTimer, this, 
			&AEnemy::ResetHitRecatTimer, 0.5f);
	}
}

void AEnemy::ResetHitRecatTimer()
{
	bCanHitReact = true;
}

void AEnemy::Die()
{
	HideHealthBar();
}

void AEnemy::StoreDamageNumber(UUserWidget* DamageNumber, FVector HitLocation)
{
	if (DamageNumber) {
 		DamageNumbers.Add(DamageNumber, HitLocation);

		FTimerHandle DamageHitTimer;
 		FTimerDelegate DamageNumberDelegate;

		DamageNumberDelegate.BindUFunction(this, FName("DestroyDamageNumber"), DamageNumber);
		GetWorldTimerManager().SetTimer(
			DamageHitTimer, 
			DamageNumberDelegate, 
			HitNumberDestroyTime, 
			false);
	}
	else {
		SHOW("DamageNumber is null in StoreDamageNumber.");
	}
}

void AEnemy::DestroyDamageNumber(UUserWidget* DamageNumber)
{
	if (DamageNumber) {
		DamageNumbers.Remove(DamageNumber);
		DamageNumber->RemoveFromParent();
	}
	else {
		SHOW("DamageNumber is null in DestroyDamageNumber.");
	}
}

void AEnemy::UpdateDamageNumbers()
{
	for (auto& DamagePair : DamageNumbers) {
		UUserWidget* DamageNumber{ DamagePair.Key };
		const FVector& Location{ DamagePair.Value };
		FVector2D ScreenPosition;

		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);
		DamageNumber->SetPositionInViewport(ScreenPosition);
	}
	
}