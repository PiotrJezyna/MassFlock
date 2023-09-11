// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockBlueprintFunctionLibrary.h"

#include "Kismet/KismetMathLibrary.h"

FVector UFlockBlueprintFunctionLibrary::DeprojectScreenPositionToGround(const APlayerController* PC, const float ScreenX, const float ScreenY, const float GroundAltitude = 0.0f)
{
	const FPlane GroundPlane = FPlane(FVector(0, 0, GroundAltitude), FVector::UpVector);
	const FVector Origin = PC->GetPawn()->GetActorLocation();
	
	FVector WorldLocation, WorldDirection;
	PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, OUT WorldLocation, OUT WorldDirection);

	float T; FVector Intersection;
	UKismetMathLibrary::LinePlaneIntersection(Origin, Origin + WorldDirection * UE_MAX_FLT, GroundPlane, OUT T, OUT Intersection);

	return Intersection;
}
