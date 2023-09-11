// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockBoundsVolume.h"

#include "FlockBoundsSubsystem.h"

AFlockBoundsVolume::AFlockBoundsVolume()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFlockBoundsVolume::BeginPlay()
{
	const FBox Box = GetBounds().GetBox();
	UFlockBoundsSubsystem* Subsystem = GetWorld()->GetSubsystem<UFlockBoundsSubsystem>();
	Subsystem->SetBounds(Box, BoundsMargin);
}