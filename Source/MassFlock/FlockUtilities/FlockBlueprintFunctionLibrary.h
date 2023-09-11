// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlockBlueprintFunctionLibrary.generated.h"

UCLASS()
class MASSFLOCK_API UFlockBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	* Convert 2D screen position into World Space 3D position placed on a ground
	* (considering that ground is flat and at given altitude)
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector DeprojectScreenPositionToGround(const APlayerController* PC, const float ScreenX, const float ScreenY, const float GroundAltitude);
};
