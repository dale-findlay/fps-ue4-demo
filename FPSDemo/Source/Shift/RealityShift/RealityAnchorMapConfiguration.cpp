// Fill out your copyright notice in the Description page of Project Settings.


#include "RealityShift/RealityAnchorMapConfiguration.h"
#include "Subsystems/RealityShiftSubsystem.h"

DEFINE_LOG_CATEGORY(LogRealityAnchorConfig);

// Sets default values
ARealityAnchorMapConfiguration::ARealityAnchorMapConfiguration()
{}

// Called when the game starts or when spawned
void ARealityAnchorMapConfiguration::BeginPlay()
{
	Super::BeginPlay();

	if (RealRealityAnchor == nullptr)
	{
		UE_LOG(LogRealityAnchorConfig, Warning, TEXT("Invalid Reality Anchor Configuration, you're missing an anchor for the real reality. The shift ability will not be available."));
	}

	if (AlternateRealityAnchor== nullptr)
	{
		UE_LOG(LogRealityAnchorConfig, Warning, TEXT("Invalid Reality Anchor Configuration, you're missing an anchor for the alternate reality. The shift ability will not be available."));
	}

	GetGameInstance()->GetSubsystem<URealityShiftSubsystem>()->SetRealityAnchors(RealRealityAnchor, AlternateRealityAnchor);
}

