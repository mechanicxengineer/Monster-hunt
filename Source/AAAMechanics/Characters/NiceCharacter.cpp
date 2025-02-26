// Fill out your copyright notice in the Description page of Project Settings.


#include "NiceCharacter.h"
#include "Camera//CameraComponent.h"
#include "GameFramework//SpringArmComponent.h"
#include "GameFramework//CharacterMovementComponent.h"

// Sets default values
ANiceCharacter::ANiceCharacter()
	: BaseTurnRate(45.0f),
	  BaseLookupRate(45.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create a camera boom(pulls in towards the character)  */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;										//	The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true;									//	Rotate the arm based on the controller
	CameraBoom->TargetOffset = FVector{ 0.0f, 0.0f, 60.0f };					//	Offset to the camera

	/**	Create a follow camera */
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	//	Attach camera to end of boom
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;								//	Camera does not rotate relative to arm

	//	Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;					//	Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator{ 0.0f, 540.0f, 0.0f };		//	At this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void ANiceCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANiceCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANiceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANiceCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANiceCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANiceCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANiceCharacter::LookupAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void ANiceCharacter::MoveForward(float _value)
{
	if ((Controller != nullptr) && _value != 0.0f) {
		/** find out which way is forward */
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, _value);
	}
}

void ANiceCharacter::MoveRight(float _value)
{
	if ((Controller != nullptr) && _value != 0.0f) {
		/** find out which way is forward */
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
	
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, _value);
	}

}

void ANiceCharacter::TurnAtRate(float rate)
{
	/** calculate delta for this frame from the rate information */
	//	deg/sec * sec/frame
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANiceCharacter::LookupAtRate(float rate)
{
	//	deg/sec * sec/frame
	AddControllerPitchInput(rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

void ANiceCharacter::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire Weapon"));
}