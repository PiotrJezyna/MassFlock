﻿// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "FlockAvoidanceTrait.generated.h"


USTRUCT()
struct FFlockAvoidanceParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	/** Maximum distance from which the Agent can be influenced by other Agents */  
	UPROPERTY(EditAnywhere, meta=(Units="cm"))
	float Radius = 100.0f;

	/** Maximum force that can be applied to the Agent */  
	UPROPERTY(EditAnywhere)
	float Force = 500.0f;

	/** Maximum neighbours that can influence agent in a single Tick. Higher value means lower framerate */ 
	UPROPERTY(EditAnywhere)
	int MaxNeighbours = 10;

	inline float SquaredRadius() const { return Radius * Radius; }
};

USTRUCT()
struct FFlockAvoidanceTag : public FMassTag
{
	GENERATED_BODY()
};

UCLASS(DisplayName = "Flock Avoidance")
class MASSFLOCK_API UFlockAvoidanceTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FFlockAvoidanceParameters Parameters;

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
