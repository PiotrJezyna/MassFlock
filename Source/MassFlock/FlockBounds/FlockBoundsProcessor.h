// Copyright Piotr Jeżyna. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "FlockBoundsProcessor.generated.h"


UCLASS()
class MASSFLOCK_API UFlockBoundsProcessor : public UMassProcessor
{
	GENERATED_BODY()

	FMassEntityQuery EntityQuery;
	
public:
	UFlockBoundsProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
};
