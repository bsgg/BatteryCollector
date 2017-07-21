// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "BatteryCollectorCharacter.h"
#include "Pickup.h"
#include "BatteryPickup.h"

//////////////////////////////////////////////////////////////////////////
// ABatteryCollectorCharacter

ABatteryCollectorCharacter::ABatteryCollectorCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// Create collection sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Set base power level for the character
	InitialPower = 2000.0f;
	CharacterPower = InitialPower;
	
	// Set the dependance of th e speed on the power lever
	SpeedFactor = 0.75f;
	BaseSpeed = 10.0f;

	// Set number collected batteries
	CollectedBatteries = 0;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABatteryCollectorCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);


	// Bind collect input
	PlayerInputComponent->BindAction("Collect", IE_Released, this, &ABatteryCollectorCharacter::CollectPickups);


	PlayerInputComponent->BindAxis("MoveForward", this, &ABatteryCollectorCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABatteryCollectorCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABatteryCollectorCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABatteryCollectorCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABatteryCollectorCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABatteryCollectorCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABatteryCollectorCharacter::OnResetVR);
}


void ABatteryCollectorCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABatteryCollectorCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ABatteryCollectorCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ABatteryCollectorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollectorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollectorCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollectorCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollectorCharacter::CollectPickups()
{
	// Get all overlapping actors
	TArray<AActor*> collectedActors;
	CollectionSphere->GetOverlappingActors(collectedActors);

	// Keep track of the collected battery power
	float collectedPower = 0;

	// Check if there is any pickup
	for (int32 iCollected = 0; iCollected < collectedActors.Num(); ++iCollected)
	{
		APickup* const testPickup = Cast<APickup>(collectedActors[iCollected]);

		// If it's a pickup and not pending of kill and is active
		// then, collect the pickup and deactivate
		if (testPickup && !testPickup->IsPendingKill() && testPickup->IsActive())
		{
			testPickup->WasCollected();

			// Check if pickup is a battery and collect the power
			ABatteryPickup* const batteryPickup = Cast<ABatteryPickup>(testPickup);
			if (batteryPickup)
			{
				collectedPower += batteryPickup->GetPower();
				// Add 1 to collected batteires
				CollectedBatteries += 1;
			}

			// Disable pickup
			batteryPickup->SetActive(false);
		}
	}

	// Update power in the character
	if (collectedPower > 0.0f)
	{
		UpdatePower(collectedPower);
	}
}

// Reports starting power
float ABatteryCollectorCharacter::GetInitialPower()
{
	return InitialPower;
}

// Reports current power
float ABatteryCollectorCharacter::GetCurrentPower()
{
	return CharacterPower;
}

// Updates the current power with PowerChange, whenever is increased of decreased
void ABatteryCollectorCharacter::UpdatePower(float PowerChange)
{
	CharacterPower = CharacterPower + PowerChange;
	// Change speed base on power
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + SpeedFactor * CharacterPower;

	// Call visual effect
	PowerChangeEffect();
}

int ABatteryCollectorCharacter::GetCurrentCollectedBatteries() const
{
	return CollectedBatteries;
}
