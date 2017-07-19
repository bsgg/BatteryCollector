// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector.h"
#include "BatteryCollectorGameMode.h"
#include "BatteryCollectorCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

ABatteryCollectorGameMode::ABatteryCollectorGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Set base decay rate
	DecayRate = 0.01f;
}

void ABatteryCollectorGameMode::BeginPlay()
{
	Super::BeginPlay();


	// Set state to playing
	SetCurrentState(EBatteryPlayState::VE_Playing);
	
	// Set the socre to beat
	// Check that we are using the battery collector chacarter
	ABatteryCollectorCharacter* character = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (character)
	{
		PowerToWin = character->GetInitialPower() * 1.25f;
	}

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}


void ABatteryCollectorGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check that we are using the battery collector chacarter
	ABatteryCollectorCharacter* character = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (character)
	{

		// Set the state of the game to win if Current power in character more than power to win the game
		if (character->GetCurrentPower() > PowerToWin)
		{
			SetCurrentState(EBatteryPlayState::VE_Won);

		}
		
		// If the character's power is positive
		else if (character->GetCurrentPower() > 0.0f)
		{
			// Decrease the character's power using the decay rate
			character->UpdatePower(-DeltaTime * DecayRate * (character->GetInitialPower()));
		}

		// Negative power, Game over
		else
		{
			SetCurrentState(EBatteryPlayState::VE_GameOver);
		}
	}

}

float ABatteryCollectorGameMode::GetPowerToWin() const
{
	return PowerToWin;
}


/** Returns the current playing state */
EBatteryPlayState ABatteryCollectorGameMode::GetCurrentState() const
{
	return CurrentState;
}

/** Set a new playing state */
void ABatteryCollectorGameMode::SetCurrentState(EBatteryPlayState NewState)
{
	CurrentState = NewState;
}
