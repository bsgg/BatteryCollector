// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "BatteryCollectorGameMode.generated.h"


// enum to store the current state of gameplay
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EBatteryPlayState : uint8
{
	VE_Playing 	UMETA(DisplayName = "Playing"),
	VE_GameOver 	UMETA(DisplayName = "GameOver"),
	VE_Won	UMETA(DisplayName = "Won"),
	VE_Unknow	UMETA(DisplayName = "Unknow")
};

UCLASS(minimalapi)
class ABatteryCollectorGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABatteryCollectorGameMode();

	virtual void Tick(float DeltaTime) override;

	/** Returns power needed to win, needed for the HUD */
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWin() const;

	virtual void BeginPlay() override;

	/** Returns the current playing state */
	UFUNCTION(BlueprintPure, Category = "Power")
	EBatteryPlayState GetCurrentState() const;

	/** Set a new playing state */
	void SetCurrentState(EBatteryPlayState NewState);


protected:

	/** The rate at which the character loses power */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float DecayRate;

	/** Power needed to win the game*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float PowerToWin;

	/** Widget class to use for our HUD Screen */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass;


	/** The instance of the HUD */
	UPROPERTY()
	class UUserWidget* CurrentWidget;

private:

	/** Keep track of the current playing state */
	EBatteryPlayState CurrentState;


};



