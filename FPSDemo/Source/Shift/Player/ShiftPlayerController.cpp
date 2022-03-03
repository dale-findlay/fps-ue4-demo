// Fill out your copyright notice in the Description page of Project Settings.
#include "ShiftPlayerController.h"
#include "Subsystems/RealityShiftSubsystem.h"
#include "Player/ShiftPlayerCharacter.h"

AShiftPlayerController::AShiftPlayerController()
{}

void AShiftPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AShiftPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("ActivateShift", EInputEvent::IE_Released, this, &AShiftPlayerController::OnActivateShift);
}

void AShiftPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	PlayerPawn = Cast<AShiftPlayerCharacter>(InPawn);
	checkf(PlayerPawn, TEXT("Player pawn was not an AShiftPlayerCharacter."));

	GetGameInstance()->GetSubsystem<URealityShiftSubsystem>()->SetTargetCharacter(this, PlayerPawn);
}

void AShiftPlayerController::OnActivateShift()
{
	GetGameInstance()->GetSubsystem<URealityShiftSubsystem>()->AttemptShift();
}
