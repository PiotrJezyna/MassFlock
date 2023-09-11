// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockBoundsSubsystem.h"


void UFlockBoundsSubsystem::SetBounds(const FBox& Bounds, const float Margins)
{
	OuterBounds = Bounds;
	
	InnerBounds = Bounds;
	InnerBounds.Max = InnerBounds.Max - FVector(Margins, Margins, 0.0f);
	InnerBounds.Min = InnerBounds.Min + FVector(Margins, Margins, 0.0f);
}

bool UFlockBoundsSubsystem::IsOutsideOfBounds(const FVector& In, FVector& OutDirection, float& OutForceScale) const
{
	UE_MT_SCOPED_READ_ACCESS(OuterBoundsAccessDetector);

	// Agent is outside of OuterBounds, it needs to steer back with max force
	if (OuterBounds.IsInsideOrOn(In) == false)
	{
		const FVector ClosestPoint = InnerBounds.GetClosestPointTo(In);
		OutDirection = (ClosestPoint - In).GetSafeNormal2D();
		OutForceScale = 1.0f;
		return true;
	}

	UE_MT_SCOPED_READ_ACCESS(InnerBoundsAccessDetector);

	// Agent is outside of InnerBounds, but still inside OuterBounds, it needs to steer back, but not exactly with max force
	if (InnerBounds.IsInsideOrOn(In) == false)
	{
		const FVector Closest = InnerBounds.GetClosestPointTo(In);
		OutDirection = (Closest - In).GetSafeNormal2D();
		
		const FVector ClosestInnerPoint = InnerBounds.GetClosestPointTo(In);
		const FVector ClosestOuterPoint = OuterBounds.GetClosestPointTo(ClosestInnerPoint + 100.0f * (In - ClosestInnerPoint));
		const FVector AB = ClosestOuterPoint - ClosestInnerPoint;
		const FVector AV = In - ClosestInnerPoint;

		OutForceScale = FVector::DotProduct(AV, AB) / FVector::DotProduct(AB, AB); // InverseLerp V between AB
		return true;
	}

	// Agent is inside InnerBounds, we dont need to intervene
	return false;
}
