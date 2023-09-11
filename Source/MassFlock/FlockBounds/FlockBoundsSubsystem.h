// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MassExternalSubsystemTraits.h"
#include "Misc/MTAccessDetector.h"
#include "FlockBoundsSubsystem.generated.h"


UCLASS()
class MASSFLOCK_API UFlockBoundsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
private:
	UE_MT_DECLARE_RW_ACCESS_DETECTOR(InnerBoundsAccessDetector);
	FBox InnerBounds;

	UE_MT_DECLARE_RW_ACCESS_DETECTOR(OuterBoundsAccessDetector);
	FBox OuterBounds;

public:
	/**
	* Sets size of the Bounds
	* @param Bounds OuterBounds
	* @param Margins Max distance from Bounds border where agent are free to move as they wish.
	* When agent gets closer to the Bounds than this value, forces start to steer him back to the center.
	*/
	void SetBounds(const FBox& Bounds, const float Margins);

	/**
	* Returns true if In Location is outside of InnerBounds
	* @param In Location to test
	* @param OutDirection If we're out of bounds, returns the direction we should move at to come back
	* @param OutForceScale Returns 1.0f when we're out of OuterBounds, 0.0f when we're inside inner bounds
	* and value in between when we're between InnerBounds and OuterBound representing the relation between distances
	* to Inner and Outer Bounds. Think of it as an Inverse Lerp of Location between InnerBounds and OuterBounds   
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsOutsideOfBounds(const FVector& In, FVector& OutDirection, float& OutForceScale) const;
};

template<>
struct TMassExternalSubsystemTraits<UFlockBoundsSubsystem> final
{
	enum
	{
		ThreadSafeRead = true,
		ThreadSafeWrite = false,
		GameThreadOnly = true,
	};
};

