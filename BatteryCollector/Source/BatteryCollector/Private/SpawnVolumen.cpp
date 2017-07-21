// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "SpawnVolumen.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickup.h"


// Sets default values
ASpawnVolumen::ASpawnVolumen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set wheretospawn as a root
	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
	RootComponent = WhereToSpawn;

	// Set spawn delay range
	SpawnDelayRangeLow = 1.0f;
	SpawnDelayRangeHigh = 4.5f;

}

// Called when the game starts or when spawned
void ASpawnVolumen::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ASpawnVolumen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolumen::GetRandomPointInVolume()
{
	FVector spawnOrigin = WhereToSpawn->Bounds.Origin;
	FVector spawnExtend = WhereToSpawn->Bounds.BoxExtent;

	// Returns a random point within the specified bounding box
	return UKismetMathLibrary::RandomPointInBoundingBox(spawnOrigin, spawnExtend);

}

// Handle spawning a new pickup
void  ASpawnVolumen::SpawnPickup()
{
	UWorld*  const world = GetWorld();
	if ((WhatToSpawn != NULL) && (world != NULL))
	{
		// Spawn parameters
		FActorSpawnParameters spawnParamenters;
		spawnParamenters.Owner = this;
		spawnParamenters.Instigator = Instigator;

		// Get random location to sapwn at
		FVector spawnLoc = GetRandomPointInVolume();

		// Random rotation
		FRotator spawnRotator;
		spawnRotator.Yaw = FMath::FRand() * 360.0f;
		spawnRotator.Pitch = FMath::FRand() * 360.0f;
		spawnRotator.Roll = FMath::FRand() * 360.0f;

		// Spawn the pickup
		APickup* const  spawnedPickup = world->SpawnActor<APickup>(WhatToSpawn, spawnLoc, spawnRotator, spawnParamenters);


		// Reset random timer to call SpawnPickup
		SpawnDelay = FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolumen::SpawnPickup, SpawnDelay, false);
		
	}
}

void ASpawnVolumen::SetSpawningActive(bool bShouldSpawn)
{
	if (bShouldSpawn)
	{
		// Set timer and bind it to SpawnPickup method
		SpawnDelay = FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolumen::SpawnPickup, SpawnDelay, false);
	}
	else
	{
		// clear the timer on spanw pickup
		GetWorldTimerManager().ClearTimer(SpawnTimer);
	}
}


