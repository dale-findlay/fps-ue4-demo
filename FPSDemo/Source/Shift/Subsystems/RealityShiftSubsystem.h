// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RealityShift/RealityState.h"
#include "RealityShiftSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SHIFT_API URealityShiftSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	URealityShiftSubsystem();

	/** Attemp a shift from the current reality to the relative alternate reality. **/
	void AttemptShift();

	/** Update the targeted character. **/
	UFUNCTION(BlueprintCallable)
	void SetTargetCharacter(class AShiftPlayerController* PlayerController, class AShiftPlayerCharacter* PlayerCharacter);

	/** Updates the targeted level sequence. **/
	void SetLevelSequence(class ULevelSequence* ShiftEffectLevelSequence);

	/** Set the respective reality anchors. **/
	void SetRealityAnchors(class ARealityAnchor* InRealityAnchor, class ARealityAnchor* InAlternateRealityAnchor);

	/** Returns true when the target player is able to shift. **/
	UFUNCTION(BlueprintPure)
	bool GetCanShift() const;

	/**
	 * Process the pending change in reality, eg teleport the player to the reality level.
	 * This event is triggered by the SequenceDirector from the level sequence.
	 */
	UFUNCTION(BlueprintCallable)
	void ProcessRealityStateChange();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection);	
	virtual void Deinitialize();

private:
	/** Execute a shift to a given reality. **/
	void ExecuteShift(ERealityState InTargetReality);
	
	/** Shift to the actual reality. **/
	void ShiftToReality();
	
	/** Shift to the alternate reality. **/
	void ShiftToAlternate();

	/** Get the cached reality anchor for a given reality state. **/
	class ARealityAnchor* GetAnchorFromRealityState(ERealityState RealityState);

	/** Calculate the target shift destination. **/
	FVector CalculateShiftDestination(ERealityState InCurrentReality, const FVector& PositionInReality, ERealityState InTargetReality);

	/** Spawn a level sequence player and actor for the given level sequence. **/
	void SpawnLevelSequencePlayer(class ULevelSequence* InLevelSequence, class ULevelSequencePlayer** OutLevelSequencePlayer, class ALevelSequenceActor** OutLevelSequenceActor);

	/** Returns the duration of the given LS player in seconds. **/
	float GetLevelSequencePlayerDurationSecs(class ULevelSequencePlayer* LevelSequencePlayer);

private:
	class AShiftPlayerController* TargetPlayerController;
	class AShiftPlayerCharacter* TargetPlayerCharacter;

	float ShiftDuration;

	bool bCurrentlyShifting;
	
	//Reality anchors.
	class ARealityAnchor* RealityAnchor;
	class ARealityAnchor* AlternateRealityAnchor;

	//Reality State stuff.
	bool bPendingRealityStateChange;
	ERealityState TargetReality;
	FVector TargetShiftDestination;

	ERealityState CurrentReality;

	//Level sequence effect stuff.
	float ShiftSequenceLength;
	class ULevelSequence* ShiftLevelSequence;
	class ALevelSequenceActor* ShiftLevelSequenceActor;
	class ULevelSequencePlayer* ShiftLevelSequencePlayer;

	FTimerHandle ShiftRealityTimerHandle;
};
