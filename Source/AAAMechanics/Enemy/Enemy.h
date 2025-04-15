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

	/** Behavior tree for the AI character */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;
	
	/** Point for the enmey to move to */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true",
		MakeEditWidget = "true"))
	FVector PatrolPoint;
	/** Point for the enmey to move to */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true",
		MakeEditWidget = "true"))
	FVector PatrolPoint2;
	
	class AEnemyController* EnemyController;

	/** Sphere component for aggro range */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgrosSphere;
	/** Sphere component for attack range */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CombatRangeSphere;

	/** True when playing the get hit animation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat",  meta = (AllowPrivateAccess = "true"))
	bool bStunned;
	/** Chance of begin stunned, 0 : no stun chance, 1 : 100% stun chance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat",  meta = (AllowPrivateAccess = "true"))
	float StunChance;
	
	/** Ture when in attack range; time to attack */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat",  meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange;

	/** Montage containing four attack animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;
	
	/** The for attack montage section names */
	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;
	
	/** Collision volume for the left weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LeftWeaponCollision;
	/** Collision volume for the right weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;
	
	/** Base damage for enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;
	
	/** Socket name for the left weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName LeftWeaponSocket;
	/** Socket name for the right weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName RightWeaponSocket;
	
	/** true when enemy can attack */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;
	
	FTimerHandle AttackWaitTImer;
	
	/** Minimum wait time between attacks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackWaitTime;
	
	/** Death anim montage for the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;
	
	bool bDying;
	FTimerHandle DeathTimer;
	/** Time after death until destory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float DeathTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* DeathParticles;

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
	/** Called When aomething overlapped with the agro sphere */
	UFUNCTION()
	void AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** Called When aomething overlapped with the attack sphere */
	UFUNCTION()
	void CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName section, float playRate = 1.0f);
	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();
	
	/** Do Damage the Character */
	void DoDamage(class ANiceCharacter*);
	/** Spawn blood effect at the hit location */
	void SpawnBlood(ANiceCharacter*, FName);
	/** Attempt to stun character */
	void StunCharacter(class ANiceCharacter*);
	
	void ResetCanAttack();

	UFUNCTION()
    void OnLeftWeaponOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;
 	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BulletHit_Implementation(FHitResult _hitResult) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, 
		 AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageNumber(int32 Damage, FVector HitLocation, bool bIsHeadShot);

	void ActivateLeftWeapon();
	void DeactivateLeftWeapon();
	void ActivateRightWeapon();
	void DeactivateRightWeapon();

	void FinishDeath();
	void DestroyEnemy();

	/********* GETTER **********/
	FORCEINLINE FString GetHeadBone() const { return HeadBone; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
