// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ItemMesh");
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(GetRootComponent());
	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidget->SetDrawSize(FVector2D(300.0f, 120.0f));
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	/* Hide Pickup Widget */
	PickupWidget->SetVisibility(false);

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

