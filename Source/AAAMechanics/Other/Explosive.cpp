// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"

#include "Kismet//GameplayStatics.h"
#include "Components//StaticMeshComponent.h"
#include "Components//SphereComponent.h"

#include "Sound//SoundCue.h"

#include "Particles//ParticleSystemComponent.h"
#include "GameFramework//Character.h"
#include "../DebugMacros.h"

AExplosive::AExplosive() :
	ExplosionDamage(400.f)
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);

}

void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult _hitResult, AActor* Shooter, AController* InstigatorController)
{
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ExplodeParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplodeParticles,
			_hitResult.Location, FRotator::ZeroRotator, true);
	}
	
	TArray<AActor*> OverlappingActors;
	OverlapSphere->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (auto Actor : OverlappingActors) {
		showargplus("Actor damaged by explosive : %s",* Actor->GetName());
		UGameplayStatics::ApplyDamage(Actor, ExplosionDamage, InstigatorController,
			 Shooter, UDamageType::StaticClass());
	}

	Destroy();
}
