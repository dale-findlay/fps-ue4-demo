#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ERealityState : uint8 {
	Reality = 0	UMETA(DisplayName = "Reality"),
	AlternateReality = 1	UMETA(DisplayName = "AlternateReality")
};