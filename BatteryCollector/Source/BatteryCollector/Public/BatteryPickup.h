// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class BATTERYCOLLECTOR_API ABatteryPickup : public APickup
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABatteryPickup();
	
	// Override the WasCollected function, use implementation because it's a blueprint native event
	void WasCollected_Implementation() override;

	// Public accessor the Battery's power
	float GetPower();


protected:

	// Set the amount of power the battery gives to the character
	// BlueprintProtected: This function can only be called on 'this' in a Blueprint. It cannot be called on another instance.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta =(BlueprintProtected = "true"))
	float BatteryPower;

	
};
