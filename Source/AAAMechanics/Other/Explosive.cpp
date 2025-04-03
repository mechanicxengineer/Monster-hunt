// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"

#include "Kismet//GameplayStatics.h"

#include "Sound//SoundCue.h"

#include "Particles//ParticleSystemComponent.h"

AExplosive::AExplosive()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult _hitResult)
{
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ExplodeParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplodeParticles,
			_hitResult.Location, FRotator::ZeroRotator, true);
	}

	// TODO apply explode damage

	Destroy();
}
