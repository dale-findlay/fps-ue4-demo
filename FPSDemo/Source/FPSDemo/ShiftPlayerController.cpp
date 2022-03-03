// Fill out your copyright notice in the Description page of Project Settings.
#include "ShiftPlayerController.h"
#include "Engine.h"
#include "TimerManager.h"

#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

#include "FPSDemo/FPSDemoCharacter.h"
#include "FPSDemo/RealityAnchor.h"

AShiftPlayerController::AShiftPlayerController()
{
	ShiftDuration = 10.f;
	RealityOffsetY = 0.0f;
	bCurrentlyShifting = false;
	bCanShift = false;
	CurrentReality = ERealityState::Reality;
}

void AShiftPlayerController::ProcessRealityStateChange()
{
	if (!bPendingRealityStateChange)
	{
		return;
	}

	//Update current reality.
	CurrentReality = TargetReality;

	//Update player position.
	PlayerPawn->SetActorLocation(TargetShiftDestination);

	//Update the state of the post fx for each reality.
	//ResetRealityPostFXState(CurrentReality);

	bPendingRealityStateChange = false;
}

void AShiftPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Spawn a level sequence actor to play the shift effect.	
	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	ShiftLevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		ShiftEffectLevelSequence,
		PlaybackSettings,
		ShiftLevelSequenceActor
	);

	ShiftSequenceLength = ShiftLevelSequencePlayer->GetEndTime().AsSeconds();

	//Compute and cache the X-Offset between the center points in each reality.
	TArray<AActor*> OutRealityAnchorActorss;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARealityAnchor::StaticClass(), OutRealityAnchorActorss);
	
	//checkf(OutRealityAnchorActorss.Num() == 2, TEXT("2 Reality Anchors MUST be present in the scene."));
	if(OutRealityAnchorActorss.Num() != 2)
	{
		bCanShift = false;
		return;
	}

	ARealityAnchor* RealityAnchor = nullptr;
	ARealityAnchor* AlternateRealityAnchor = nullptr;
	for (const auto& AnchorActor : OutRealityAnchorActorss)
	{
		ARealityAnchor* Anchor = Cast<ARealityAnchor>(AnchorActor);

		if (Anchor->RealityState == ERealityState::Reality)
		{
			RealityAnchor = Anchor;
		}
		else if (Anchor->RealityState == ERealityState::AlternateReality)
		{
			AlternateRealityAnchor = Anchor;
		}
	}

	//NOTE: this is a REALLY naive implementation...
	//In reality i would obviously make this way more robust, but this is quick and dirty weekend stuff.
	//checkf(AlternateRealityAnchor && RealityAnchor, TEXT("You must have at least one Reality and one Alternate reality anchor in the scene."));
	if (!(AlternateRealityAnchor && RealityAnchor))
	{
		bCanShift = false;
		return;
	}
	FVector RealityToAlternateReality = RealityAnchor->GetActorLocation() - AlternateRealityAnchor->GetActorLocation();
	RealityOffsetY = RealityToAlternateReality.Size();
}

void AShiftPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ActivateShift", EInputEvent::IE_Released, this, &AShiftPlayerController::OnActivateShift);
}

void AShiftPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerPawn = Cast<AFPSDemoCharacter>(InPawn);
	checkf(PlayerPawn, TEXT("Player pawn was not an AFPSDemoCharacter."));
}

void AShiftPlayerController::OnActivateShift()
{
	if (!bCanShift)
	{
		return;
	}

	if (bCurrentlyShifting)
	{
		return;
	}	

	if (CurrentReality == ERealityState::Reality)
	{
		//Set the timer to shift back to reality.
		GetWorld()->GetTimerManager().SetTimer(ShiftRealityTimerHandle, this, &AShiftPlayerController::ShiftToReality, ShiftDuration, false);
		ShiftToAlternate();
	}
	else if (CurrentReality == ERealityState::AlternateReality)
	{
		//reset the timer because we shifted early.
		GetWorld()->GetTimerManager().ClearTimer(ShiftRealityTimerHandle);
		ShiftToReality();
	}

}

void AShiftPlayerController::ExecuteShift(ERealityState InTargetReality)
{
	check(ShiftLevelSequencePlayer && "Level sequence player was invalid!, begin play isn't getting called?");

	bCurrentlyShifting = true;

	TargetReality = InTargetReality;
	TargetShiftDestination = CalculateShiftDestination(TargetReality, PlayerPawn->GetActorLocation());

	bPendingRealityStateChange = true;

	//Jump back to frame 0 and play.
	FMovieSceneSequencePlaybackParams PlaybackParams(0, EUpdatePositionMethod::Jump);
	ShiftLevelSequencePlayer->SetPlaybackPosition(PlaybackParams);
	ShiftLevelSequencePlayer->Play();

	PlayerPawn->DisableMovement();

	FTimerDelegate ShiftFinishedCallback;
	ShiftFinishedCallback.BindLambda([this]{
		bCurrentlyShifting = false;
		PlayerPawn->EnableMovement();
		ShiftLevelSequencePlayer->Stop();
	});

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, ShiftFinishedCallback, ShiftSequenceLength, false);
}

FVector AShiftPlayerController::CalculateShiftDestination(ERealityState InTargetReality, const FVector& PositionInReality)
{
	//Apply the pre-computed reality offset to transform the position to the correct reality.
	if (TargetReality == ERealityState::Reality)
	{
		return PositionInReality - FVector(0, RealityOffsetY, 0);
	}

	return PositionInReality + FVector(0, RealityOffsetY, 0);
}

void AShiftPlayerController::ShiftToReality()
{
	ExecuteShift(ERealityState::Reality);
}

void AShiftPlayerController::ShiftToAlternate()
{
	ExecuteShift(ERealityState::AlternateReality);
}
