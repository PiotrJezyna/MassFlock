// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockBoundsProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassMovementFragments.h"
#include "FlockBoundsTrait.h"
#include "FlockBoundsSubsystem.h"
#include "MassFlock/FlockProcessor/FlockProcessor.h"
#include "MassFlock/FlockUtilities/FlockUtilities.h"


UFlockBoundsProcessor::UFlockBoundsProcessor()
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteAfter.Add(UE::MassFlock::ProcessorGroupNames::Flock);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
}

void UFlockBoundsProcessor::ConfigureQueries()
{
	EntityQuery.AddConstSharedRequirement<FFlockBoundsParameters>(EMassFragmentPresence::All);
	
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassForceFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);

	EntityQuery.AddSubsystemRequirement<UMassNavigationSubsystem>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<UFlockBoundsSubsystem>(EMassFragmentAccess::ReadOnly);
	
	EntityQuery.RegisterWithProcessor(*this);
}

void UFlockBoundsProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, &EntityManager, World = EntityManager.GetWorld()](FMassExecutionContext& Context)
	{
		const UFlockBoundsSubsystem& BoundsSubsystem = Context.GetSubsystemChecked<UFlockBoundsSubsystem>(World);
		const FFlockBoundsParameters Parameters = Context.GetConstSharedFragment<FFlockBoundsParameters>();
		
		const TConstArrayView<FTransformFragment> TransformFragments = Context.GetFragmentView<FTransformFragment>();
		const TArrayView<FMassForceFragment> ForceFragments = Context.GetMutableFragmentView<FMassForceFragment>();
		const TArrayView<FMassVelocityFragment> VelocityFragments = Context.GetMutableFragmentView<FMassVelocityFragment>();
		
		for (int i = 0; i < Context.GetNumEntities(); i++)
		{
			FTransform Transform = TransformFragments[i].GetTransform();
			FVector Location = Transform.GetLocation();
		
			FMassForceFragment& Force = ForceFragments[i];
			FVector& CurrentForce = Force.Value;

			FVector Direction; float ForceScale;
			if (BoundsSubsystem.IsOutsideOfBounds(Location, OUT Direction, OUT ForceScale))
			{
				CurrentForce += Direction * Parameters.Force * ForceScale;
			}
		}
	});
}
