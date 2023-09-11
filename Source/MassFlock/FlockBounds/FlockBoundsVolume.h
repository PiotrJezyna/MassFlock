// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FlockBoundsVolume.generated.h"

UCLASS()
class MASSFLOCK_API AFlockBoundsVolume : public AVolume
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float BoundsMargin = 3000.0f;
	
public:
	AFlockBoundsVolume();
	
protected:
	virtual void BeginPlay() override;
};