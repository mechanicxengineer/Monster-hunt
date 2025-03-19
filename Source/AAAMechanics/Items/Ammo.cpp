// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "..//Characters/NiceCharacter.h"

AAmmo::AAmmo()
{

    /** Contruct the ammo mesh component and set it as the root */
    AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
    SetRootComponent(AmmoMesh);

    GetCollisionBox()->SetupAttachment(GetRootComponent());
    GetPickupWidget()->SetupAttachment(GetRootComponent());
    GetAreaSphere()->SetupAttachment(GetRootComponent());

	AmmoCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AmmoCollisionSphere"));
	AmmoCollisionComponent->SetupAttachment(GetRootComponent());
	AmmoCollisionComponent->SetSphereRadius(50.f);
}

void AAmmo::BeginPlay()
{
    Super::BeginPlay();

	AmmoCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::AmmoSphereOverlap);
}

void AAmmo::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAmmo::SetItemProperties(EItemState State)
{
    Super::SetItemProperties(State);

    switch (State) {
		case EItemState::EIS_PICKUP:
			/** Set mesh and area sphere properties */
			AmmoMesh->SetSimulatePhysics(false);
			AmmoMesh->SetEnableGravity(false);
			AmmoMesh->SetVisibility(true);
			AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;
		case EItemState::EIS_EQUIPPED:
			/** Set mesh and area sphere properties */
			AmmoMesh->SetSimulatePhysics(false);
			AmmoMesh->SetEnableGravity(false);
			AmmoMesh->SetVisibility(true);
			AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;
		case EItemState::EIS_FALLING:
			/** Set mesh and area sphere properties */
			AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			AmmoMesh->SetSimulatePhysics(true);
			AmmoMesh->SetEnableGravity(true);
			AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

			break;
		case EItemState::EIS_EQUIPINTERPING:
			/** Set mesh and area sphere properties */
			AmmoMesh->SetSimulatePhysics(false);
			AmmoMesh->SetEnableGravity(false);
			AmmoMesh->SetVisibility(true);
			AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;
		default:
			break;
	} 
}

void AAmmo::AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		if (auto character = Cast<ANiceCharacter>(OtherActor)) {
			StartItemCurve(character);
			AmmoCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}