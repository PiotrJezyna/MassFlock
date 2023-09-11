// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassNavigationSubsystem.h"
#include "MassProcessor.h"
#include "FlockProcessor.generated.h"

UCLASS()
class MASSFLOCK_API UFlockProcessor : public UMassProcessor
{
	GENERATED_BODY()

	UMassNavigationSubsystem* NavigationSubsystem;
	FMassEntityQuery EntityQuery;

protected:
	UFlockProcessor();

public:
	virtual void Initialize(UObject& Owner) override;

protected:
	virtual void ConfigureQueries() override;

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	virtual float GetMaxRadius(FMassExecutionContext& Context) const;

	virtual int GetMaxNeighbours(FMassExecutionContext& Context) const;
};
