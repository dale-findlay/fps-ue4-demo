#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSDemo/RealityState.h"

#include "RealityAnchor.generated.h"

UCLASS(BlueprintType)
class ARealityAnchor : public AActor
{
	GENERATED_BODY()

public:
	ARealityAnchor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ERealityState RealityState;
};