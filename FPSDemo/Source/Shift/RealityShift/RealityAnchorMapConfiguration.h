// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealityAnchorMapConfiguration.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRealityAnchorConfig, Log, All);

UCLASS()
class SHIFT_API ARealityAnchorMapConfiguration : public AActor
{
	GENERATED_BODY()	

public:	
	ARealityAnchorMapConfiguration();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class ARealityAnchor* RealRealityAnchor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class ARealityAnchor* AlternateRealityAnchor;
};
