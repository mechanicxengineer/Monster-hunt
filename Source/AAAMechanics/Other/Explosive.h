// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "..//Interfaces//BulletHitInterface.h"
#include "Explosive.generated.h"

UCLASS()
class AAAMECHANICS_API AExplosive : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()
	
	/** Explosive when hit by a bullet */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ExplodeParticles;
	/** send to play when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

public:	
	AExplosive();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void BulletHit_Implementation(FHitResult _hitResult) override;

};
