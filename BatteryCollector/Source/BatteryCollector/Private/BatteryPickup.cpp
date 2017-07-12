// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "BatteryPickup.h"


ABatteryPickup::ABatteryPickup()
{
	// Set the PickupMesh base mesh 
	GetMesh()->SetSimulatePhysics(true);

	// The base power lever of the battery
	BatteryPower = 150.0f;
}


void ABatteryPickup::WasCollected_Implementation()
{
	Super::WasCollected_Implementation();

	// Destroy the battery
	Destroy();
}

float ABatteryPickup::GetPower()
{
	return BatteryPower;
}

