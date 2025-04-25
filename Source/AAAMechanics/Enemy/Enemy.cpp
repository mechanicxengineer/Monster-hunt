// Fill out your copyright notice in the Description page of Project Settings.

/** Engine */
#include "Enemy.h"

#include "Engine//SkeletalMeshSocket.h"
#include "Kismet//GameplayStatics.h"
#include "Kismet//KismetMathLibrary.h"
#include "Sound//SoundCue.h"
#include "Particles//ParticleSystemComponent.h"
#include "Blueprint//UserWidget.h"
#include "BehaviorTree//BlackboardComponent.h"
#include "Components//SphereComponent.h"
#include "Components//CapsuleComponent.h"
#include "Components//BoxComponent.h"

//	Custom
#include "..//Controllers//AI//EnemyController.h"
#include "..//Characters//NiceCharacter.h"

/** Custom Debug */
#include "..//Common//AdvLog.h"
#include "..//DebugMacros.h"

// Sets default values
AEnemy::AEnemy() :
	Health(500.f),
	MaxHealth(500.f),
	HealthBarDisplayTime(3.f),
	HitReactTimeMin(0.5f),
	HitReactTimeMax(2.0f),
	bCanHitReact(true),
	HitNumberDestroyTime(1.5f),
	bStunned(false),
	StunChance(0.5f),
	AttackLFast(TEXT("AttackLFast")),
	AttackRFast(TEXT("AttackRFast")),
	AttackL(TEXT("AttackL")),
	AttackR(TEXT("AttackR")),
	LeftWeaponSocket(TEXT("FX_Trail_L_01")),
	RightWeaponSocket(TEXT("FX_Trail_R_01")),
	bCanAttack(true),
	AttackWaitTime(0.5f),
	bDying(false),
	DeathTime(4.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	/** creat the agro sphere */
	AgrosSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgrosSphere"));
	AgrosSphere->SetupAttachment(GetRootComponent());
	AgrosSphere->SetSphereRadius(200.f);
	/** create the combat range sphere */
	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangesSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());
	CombatRangeSphere->SetSphereRadius(100.f);

	/** Construct left and right weapon collision boxes */
	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollision"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("leftWeaponBone"));

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollision"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("rightWeaponBone"));

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgrosSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereBeginOverlap);

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeBeginOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);

	/** Bind function to overlap events for weapon boxes */
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);

	/** Ignore the camera for mesh and capsule */
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	/** Set Collision presets for weapon boxes */
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	/** Get the AI Controller */
	EnemyController = Cast<AEnemyController>(GetController());
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
	}
	
	const FVector WorldPatrolPoint{ UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint) };
	const FVector WorldPatrolPoint2{ UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2) };
	// draw_sphere(WorldPatrolPoint, 50.f, 12.0f);
	// draw_sphere(WorldPatrolPoint2, 50.f, 12.0f);
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);
	}

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

void AEnemy::BulletHit_Implementation(FHitResult _hitResult, AActor* Shooter, AController* InstigatorController)
{
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticles,
			_hitResult.Location, FRotator::ZeroRotator, true);
	}
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (EnemyController) {
		 EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), DamageCauser);
	}

	if (Health - DamageAmount <= 0.f) {
		Health = 0.f;
		Die();
	}
	else {
		Health -= DamageAmount;
	}

	if (bDying) return DamageAmount;
	ShowHealthBar();
	
	/** Determine wethere bullet hit stuns */
	const float Stunned = FMath::FRandRange(0.f, 1.f);
	if (Stunned <= StunChance) {
		/** stun the Enemy */
		PlayHitMontage(FName("HitFront"));
		SetStunned(true);
	}

	return DamageAmount;
}

void AEnemy::Die()
{
	//SHOW("Enemy died");
	if (bDying) return;	
	bDying = true;

	HideHealthBar();
	auto* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && DeathMontage) {
		animInstance->Montage_Play(DeathMontage);
	}
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Dead"), true);
		EnemyController->StopMovement();
	}
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

void AEnemy::StoreDamageNumber(UUserWidget* DamageNumber, FVector HitLocation)
{
	if (DamageNumber) {
 		DamageNumbers.Add(DamageNumber, HitLocation);

		FTimerHandle DamageHitTimer;
 		FTimerDelegate DamageNumberDelegate;
		/** Bind the damage number system */
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

void AEnemy::AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	auto Character = Cast<ANiceCharacter>(OtherActor);
	if (Character) {
		if (EnemyController) {
			if (EnemyController->GetBlackboardComponent()) {
				EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
			}
		}
	}
}

void AEnemy::CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return; 
	if (auto Character = Cast<ANiceCharacter>(OtherActor)) {
		bInAttackRange = true;
		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return; 
	if (auto Character = Cast<ANiceCharacter>(OtherActor)) {
		bInAttackRange = false;
		if (EnemyController) {
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), bStunned);
	}
}

void AEnemy::PlayAttackMontage(FName section, float playRate)
{
	if (auto animInstance = GetMesh()->GetAnimInstance()) {
		if (AttackMontage) {
			animInstance->Montage_Play(AttackMontage, playRate);
			animInstance->Montage_JumpToSection(section, AttackMontage);
		}
	}
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackWaitTImer, this, &AEnemy::ResetCanAttack, 
		AttackWaitTime);
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);
	}
}

FName AEnemy::GetAttackSectionName()
{	
	FName sectionName;
	const int32 Section{ FMath::RandRange(1, 4) };
	switch (Section)
	{
	case 1:
		sectionName = AttackLFast;
		break;
	case 2:
		sectionName = AttackRFast;
		break;
	case 3:
		sectionName = AttackL;
		break;
	case 4:
		sectionName = AttackR;
		break;
	}
	return sectionName;
}

void AEnemy::DoDamage(ANiceCharacter* Victim)
{
	if (Victim == nullptr) return;
	if (bInAttackRange) {
		UGameplayStatics::ApplyDamage(Victim, BaseDamage, GetController(),
									  this, UDamageType::StaticClass());
	}
	if (Victim->GetMeleeImpactSound()) {
		UGameplayStatics::PlaySoundAtLocation(this, Victim->GetMeleeImpactSound(), GetActorLocation());
		//show("Play melee impact sound");
	}
}

void AEnemy::SpawnBlood(ANiceCharacter* Victim, FName SocketName)
{
	if (const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName(SocketName)) {
		const FTransform SocketTransform = TipSocket->GetSocketTransform(GetMesh());
		if (Victim->GetBloodParticle()) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Victim->GetBloodParticle(), 
				SocketTransform);
		}
	}
}

void AEnemy::StunCharacter(ANiceCharacter* Victim)
{
	if (Health <= 0.0f) return;
	if (Victim) {
		const float stun{ FMath::FRandRange(0.0f, 1.0f) };
		if (stun <= Victim->GetStunChance()) {
			Victim->Stun();
		}
	}
}

void AEnemy::ResetCanAttack() 
{
	bCanAttack = true;
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
	}
}

void AEnemy::FinishDeath() 
{
	GetMesh()->bPauseAnims = true;
	if (DeathParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles,
			GetActorLocation(), FRotator::ZeroRotator, true);
	}
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::DestroyEnemy, DeathTime);
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto* Character = Cast<ANiceCharacter>(OtherActor)) {
		DoDamage(Character);
		SpawnBlood(Character, LeftWeaponSocket);
		StunCharacter(Character);
	}
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto* Character = Cast<ANiceCharacter>(OtherActor)) {
		DoDamage(Character);
		SpawnBlood(Character, RightWeaponSocket);
		StunCharacter(Character);
	}
}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}