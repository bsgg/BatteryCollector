// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "SpawnVolumen.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ASpawnVolumen::ASpawnVolumen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	// Set wheretospawn as a root
	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
	RootComponent = WhereToSpawn;

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

