// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector.h"
#include "BatteryCollectorGameMode.h"
#include "BatteryCollectorCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SpawnVolumen.h"

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

	// Find all spawn volumen actors
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolumen::StaticClass(), foundActors);
	for (auto actor : foundActors)
	{
		ASpawnVolumen* spanwVolumeActor = Cast<ASpawnVolumen>(actor);
		int numberSpawnVolumen = 0;
		if (spanwVolumeActor)
		{
			numberSpawnVolumen++;
			UE_LOG(LogClass, Warning, TEXT("[ABatteryCollectorGameMode]Add new spawnvolumen"));
			SpawnVolumeActors.AddUnique(spanwVolumeActor);
		}

		UE_LOG(LogClass, Warning, TEXT("[ABatteryCollectorGameMode]Number spawn volumens %i"), numberSpawnVolumen);
	}


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

	// Handles the new state
	HandleNewState(CurrentState);
}

void ABatteryCollectorGameMode::HandleNewState(EBatteryPlayState NewState)
{
	switch (NewState)
	{
		// Game is playing 
		case EBatteryPlayState::VE_Playing:
		{
			// Spawn volumens actives
			for (ASpawnVolumen* volumen : SpawnVolumeActors)
			{
				UE_LOG(LogClass, Warning, TEXT("[HandleNewState]enable volumen"));
				volumen->SetSpawningActive(true);
			}

		}
		break;

		// We won the game
		case EBatteryPlayState::VE_Won:
		{
			// Spawn volumens inactives
			for (ASpawnVolumen* volumen : SpawnVolumeActors)
			{
				volumen->SetSpawningActive(false);
			}
		}
		break;

		// We lost the game
		case EBatteryPlayState::VE_GameOver:
		{
			// Spawn volumens inactives
			for (ASpawnVolumen* volumen : SpawnVolumeActors)
			{
				volumen->SetSpawningActive(false);
			}

			// Block player input through SetCinematicMode
			APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
			if (playerController)
			{
				playerController->SetCinematicMode(true, false, false, true, true);
			}
			
			// Ragdoll the chracter			
			ACharacter* character = UGameplayStatics::GetPlayerCharacter(this, 0);
			if (character)
			{
				character->GetMesh()->SetSimulatePhysics(true);
				character->GetMovementComponent()->MovementState.bCanJump = false;
			}

		}
		break;

		// Unknown and default
		default:
		case EBatteryPlayState::VE_Unknow:
		{
			// Don´t do anything
		}
		break;
	}

}
