// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "FlockBoundsTrait.generated.h"


USTRUCT()
struct FFlockBoundsParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	/** Maximum force that can be apply to an Agent to prevent him from exiting Bounds */
	UPROPERTY(EditAnywhere)
	float Force = 500.0f;
};

UCLASS(DisplayName = "Flock Bounds")
class MASSFLOCK_API UFlockBoundsTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FFlockBoundsParameters Parameters;

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};