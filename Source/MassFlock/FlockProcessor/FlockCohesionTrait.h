// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "FlockCohesionTrait.generated.h"

USTRUCT()
struct FFlockCohesionParameters : public FMassSharedFragment
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
struct FFlockCohesionTag : public FMassTag
{
	GENERATED_BODY()
};

UCLASS(DisplayName = "Flock Cohesion")
class MASSFLOCK_API UFlockCohesionTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FFlockCohesionParameters Parameters;

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
