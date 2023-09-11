// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "FlockMovementProcessor.generated.h"

UCLASS()
class MASSFLOCK_API UFlockMovementProcessor : public UMassProcessor
{
	GENERATED_BODY()

protected:
	FMassEntityQuery EntityQuery;

public:
	UFlockMovementProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
};
