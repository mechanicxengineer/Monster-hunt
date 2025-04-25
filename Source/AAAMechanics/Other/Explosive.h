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

	/** Mesh for the explosive */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ExplosiveMesh;
	
	/** used to dertermine what actor overlap during explosion */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ExplosionDamage;

public:	
	AExplosive();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void BulletHit_Implementation(FHitResult _hitResult, AActor* Shooter, AController* InstigatorController) override;

};
