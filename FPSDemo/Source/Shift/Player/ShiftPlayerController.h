// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RealityShift/RealityState.h"
#include "ShiftPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHIFT_API AShiftPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShiftPlayerController();

protected:
	void BeginPlay() override;
	void SetupInputComponent() override;
	void OnPossess(APawn* InPawn) override;
	
private:
	/** Action Bindings **/
	void OnActivateShift();

private:
	class AShiftPlayerCharacter* PlayerPawn;
};
