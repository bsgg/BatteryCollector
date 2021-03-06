// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "Pickup.h"


// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bIsActive = true;
	
	// Set pickup mesh as a root
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Returns active state
bool APickup::IsActive()
{
	return bIsActive;
}


// Sets active state
void APickup::SetActive(bool NewPickupState)
{
	bIsActive = NewPickupState;
}

void APickup::WasCollected_Implementation()
{
	// Debug a debug message
	/*FString pickupName = GetName();
	UE_LOG(LogClass, Warning, TEXT("[Pickup] You have collected %s "), *pickupName);*/
}

