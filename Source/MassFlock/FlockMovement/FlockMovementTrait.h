// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "FlockMovementTrait.generated.h"


USTRUCT()
struct FFlockMovementParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(ClampMin=0, Units="m/s"))
	float MinSpeed = 100.0f;

	UPROPERTY(EditAnywhere, meta=(ClampMin=0, Units="m/s"))
	float MaxSpeed = 300.0f;

	UPROPERTY(EditAnywhere)
	bool RotateTowardsMovement = true;

	float MaxSpeedSquared() const
	{
		return MaxSpeed * MaxSpeed;
	}
};

UCLASS(DisplayName="Flock Movement")
class MASSFLOCK_API UFlockMovementTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FFlockMovementParameters Parameters; 

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
