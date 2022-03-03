// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/RealityShiftSubsystem.h"
#include "Engine.h"
#include "TimerManager.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

#include "Player/ShiftPlayerCharacter.h"
#include "RealityShift/RealityAnchor.h"

URealityShiftSubsystem::URealityShiftSubsystem()
{
	ShiftDuration = 10.f;
	bCurrentlyShifting = false;
	CurrentReality = ERealityState::Reality;
}

void URealityShiftSubsystem::AttemptShift()
{
	//Check that we can actually shift.
	if (!GetCanShift())
	{
		return;
	}

	//Attempt to shift to the relative alternate reality.
	if (CurrentReality == ERealityState::Reality)
	{		
		GetWorld()->GetTimerManager().SetTimer(ShiftRealityTimerHandle, this, &URealityShiftSubsystem::ShiftToReality, ShiftDuration, false);
		ShiftToAlternate();
	}
	else if (CurrentReality == ERealityState::AlternateReality)
	{
		GetWorld()->GetTimerManager().ClearTimer(ShiftRealityTimerHandle);
		ShiftToReality();
	}
}

void URealityShiftSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URealityShiftSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void URealityShiftSubsystem::SetTargetCharacter(AShiftPlayerController* PlayerController, AShiftPlayerCharacter* PlayerCharacter)
{
	TargetPlayerController = PlayerController;
	TargetPlayerCharacter = PlayerCharacter;
}

void URealityShiftSubsystem::SetLevelSequence(ULevelSequence* ShiftEffectLevelSequence)
{
	//Update the cached level sequence and create a new player.
	ShiftLevelSequence = ShiftEffectLevelSequence;

	//Clean up any existing LS players and or actors.
	if (ShiftLevelSequencePlayer != nullptr)
	{
		ShiftLevelSequencePlayer = nullptr;

		if (ShiftLevelSequenceActor != nullptr)
		{
			ShiftLevelSequenceActor->Destroy();
			ShiftLevelSequenceActor = nullptr;
		}
	}

	//Spawn the new player and update cached sequence length.
	SpawnLevelSequencePlayer(ShiftLevelSequence, &ShiftLevelSequencePlayer, &ShiftLevelSequenceActor);
	ShiftSequenceLength = GetLevelSequencePlayerDurationSecs(ShiftLevelSequencePlayer);
}

void URealityShiftSubsystem::SetRealityAnchors(ARealityAnchor* InRealityAnchor, ARealityAnchor* InAlternateRealityAnchor)
{
	RealityAnchor = InRealityAnchor;
	AlternateRealityAnchor = InAlternateRealityAnchor;
}

bool URealityShiftSubsystem::GetCanShift() const
{
	bool ValidRealityAnchors = RealityAnchor != nullptr && AlternateRealityAnchor != nullptr;
	bool ValidLevelSequenceConfig = ShiftLevelSequence != nullptr && ShiftLevelSequenceActor != nullptr && ShiftLevelSequencePlayer != nullptr;
	bool ValidTargetPlayer = TargetPlayerCharacter != nullptr && TargetPlayerController != nullptr;
	bool ValidCurrentShiftState = bCurrentlyShifting == false;

	return ValidRealityAnchors && ValidRealityAnchors && ValidTargetPlayer && ValidCurrentShiftState;
}

void URealityShiftSubsystem::ProcessRealityStateChange()
{
	if (!bPendingRealityStateChange)
	{
		return;
	}

	//Update our current reality state and move the player respectively.
	CurrentReality = TargetReality;
	TargetPlayerCharacter->SetActorLocation(TargetShiftDestination);
	//ResetRealityPostFXState(CurrentReality);

	bPendingRealityStateChange = false;
}

void URealityShiftSubsystem::ExecuteShift(ERealityState InTargetReality)
{
	check(ShiftLevelSequencePlayer && "Level sequence player was invalid!");

	bCurrentlyShifting = true;

	//Update our target reality state (where we want to go)
	bPendingRealityStateChange = true;
	TargetReality = InTargetReality;
	TargetShiftDestination = CalculateShiftDestination(CurrentReality, TargetPlayerCharacter->GetActorLocation(), TargetReality);
		
	//Reset and play the level sequence for the shift effect.
	FMovieSceneSequencePlaybackParams PlaybackParams(0, EUpdatePositionMethod::Jump);
	ShiftLevelSequencePlayer->SetPlaybackPosition(PlaybackParams);
	ShiftLevelSequencePlayer->Play();

	TargetPlayerCharacter->DisableMovement();

	//Execute a timer to wait for the shift effect to finish executing and cleanup it's state.
	FTimerDelegate ShiftFinishedCallback;
	ShiftFinishedCallback.BindLambda([this] {
		bCurrentlyShifting = false;
		TargetPlayerCharacter->EnableMovement();
		ShiftLevelSequencePlayer->Pause();
		});

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, ShiftFinishedCallback, ShiftSequenceLength, false);
}

void URealityShiftSubsystem::ShiftToReality()
{
	ExecuteShift(ERealityState::Reality);
}

void URealityShiftSubsystem::ShiftToAlternate()
{
	ExecuteShift(ERealityState::AlternateReality);
}

ARealityAnchor* URealityShiftSubsystem::GetAnchorFromRealityState(ERealityState RealityState)
{
	switch (RealityState)
	{
		case ERealityState::Reality: 
			return RealityAnchor;
		
		case ERealityState::AlternateReality:
			return RealityAnchor;
	default:
		return nullptr;
	}
}

FVector URealityShiftSubsystem::CalculateShiftDestination(ERealityState InCurrentReality, const FVector& PositionInReality, ERealityState InTargetReality)
{
	//Calculate the transformation from the given reality state.
	ARealityAnchor* CurrentRealityAnchor = GetAnchorFromRealityState(CurrentReality);
	ARealityAnchor* OppositeRealityAnchor = GetAnchorFromRealityState(TargetReality);

	//TODO: Gotta cache these.
	FVector OffsetVector = CurrentRealityAnchor->GetActorLocation() - OppositeRealityAnchor->GetActorLocation();

	return PositionInReality + OffsetVector;
}

void URealityShiftSubsystem::SpawnLevelSequencePlayer(class ULevelSequence* InLevelSequence, class ULevelSequencePlayer** OutLevelSequencePlayer, class ALevelSequenceActor** OutLevelSequenceActor)
{
	//Spawn a level sequence actor to play the shift effect.	
	FMovieSceneSequencePlaybackSettings PlaybackSettings;	
	*OutLevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		InLevelSequence,
		PlaybackSettings,
		*OutLevelSequenceActor
	);
}

float URealityShiftSubsystem::GetLevelSequencePlayerDurationSecs(ULevelSequencePlayer* LevelSequence)
{
	return LevelSequence->GetEndTime().AsSeconds();
}
