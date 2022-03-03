// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSDemo/RealityState.h"
#include "ShiftPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSDEMO_API AShiftPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShiftPlayerController();

	/**
	 * Process the pending change in reality, eg teleport the player to the reality level.
	 * This event is triggered by the SequenceDirectory from the level sequence.
	 */
	UFUNCTION(BlueprintCallable)
	void ProcessRealityStateChange();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShiftDuration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class ULevelSequence* ShiftEffectLevelSequence;

protected:
	void BeginPlay() override;
	void SetupInputComponent() override;
	void OnPossess(APawn* InPawn) override;
	
private:
	/**
	 * Play the shift effect using the cached level sequence.
	 */
	void ExecuteShift(ERealityState InTargetReality);

	FVector CalculateShiftDestination(ERealityState InTargetReality, const FVector& PositionInReality);

	void ShiftToReality();
	void ShiftToAlternate();

	/**
	 * (ACTION Binding) Invoked when the user presses the "ActivateShift" key/s
	 */
	void OnActivateShift();

private:
	class AFPSDemoCharacter* PlayerPawn;

	bool bCanShift;
	bool bCurrentlyShifting;
	float RealityOffsetY;
	ERealityState CurrentReality;

	bool bPendingRealityStateChange;
	ERealityState TargetReality;
	FVector TargetShiftDestination;

	float ShiftSequenceLength;
	class ALevelSequenceActor* ShiftLevelSequenceActor;
	class ULevelSequencePlayer* ShiftLevelSequencePlayer;

	FTimerHandle ShiftRealityTimerHandle;
};
