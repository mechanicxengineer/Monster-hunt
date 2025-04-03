// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "..//Interfaces//BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class AAAMECHANICS_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

	/** Particle to spawn when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactParticles;
	/** send to play when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	/** Current health of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health;
	/** Maximum health of the nemey */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	/** Name for the headbone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FString HeadBone;

	/** Time to display health bar once shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;
	FTimerHandle HealthBarDisplayTimer;
	
	/** Montage containing hit and death animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;
	FTimerHandle HitReactTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMax;
	
	bool bCanHitReact;
	
	/** map to store hitnumber and their hit location */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> DamageNumbers;
	
	/** Time before a hitnumber is removed from the screen*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;

public:
	AEnemy();

protected:

	/********* FUNCTIONS **********/
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintNativeEvent) void ShowHealthBar();
	void ShowHealthBar_Implementation();
	UFUNCTION(BlueprintImplementableEvent) void HideHealthBar();

	void PlayHitMontage(FName section, float playRate = 1.0f);
	void ResetHitRecatTimer();
	void Die();
	 
	UFUNCTION(BlueprintCallable)
	void StoreDamageNumber(UUserWidget* DamageNumber, FVector HitLocation);
	UFUNCTION()
	void DestroyDamageNumber(UUserWidget* DamageNumber);

	void UpdateDamageNumbers();

public:	
	virtual void Tick(float DeltaTime) override;
 	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BulletHit_Implementation(FHitResult _hitResult) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, 
		 AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageNumber(int32 Damage, FVector HitLocation, bool bIsHeadShot);

	/********* GETTER **********/
	FORCEINLINE FString GetHeadBone() const { return HeadBone; }
};
