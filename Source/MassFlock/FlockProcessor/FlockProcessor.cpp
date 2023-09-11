// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockProcessor.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassEntityView.h"
#include "MassMovementFragments.h"
#include "MassSimulationLOD.h"
#include "FlockAlignmentTrait.h"
#include "FlockAvoidanceTrait.h"
#include "FlockCohesionTrait.h"
#include "MassFlock/FlockUtilities/FlockUtilities.h"

UFlockProcessor::UFlockProcessor()
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteInGroup = UE::MassFlock::ProcessorGroupNames::Flock;
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
}

void UFlockProcessor::Initialize(UObject& Owner)
{
	NavigationSubsystem = UWorld::GetSubsystem<UMassNavigationSubsystem>(Owner.GetWorld());
}

void UFlockProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassForceFragment>(EMassFragmentAccess::ReadWrite);
	
	EntityQuery.AddTagRequirement<FFlockCohesionTag>(EMassFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FFlockCohesionParameters>(EMassFragmentPresence::Optional);

	EntityQuery.AddTagRequirement<FFlockAlignmentTag>(EMassFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FFlockAlignmentParameters>(EMassFragmentPresence::Optional);

	EntityQuery.AddTagRequirement<FFlockAvoidanceTag>(EMassFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FFlockAvoidanceParameters>(EMassFragmentPresence::Optional);
	
	EntityQuery.AddRequirement<FMassSimulationLODFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	
	EntityQuery.AddRequirement<FMassSimulationVariableTickFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddChunkRequirement<FMassSimulationVariableTickChunkFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.SetChunkFilter(&FMassSimulationVariableTickChunkFragment::ShouldTickChunkThisFrame);

	EntityQuery.RegisterWithProcessor(*this);
}

void UFlockProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, &EntityManager](FMassExecutionContext& Context)
	{
		const bool DoesHaveCohesionTag = Context.DoesArchetypeHaveTag<FFlockCohesionTag>();
		const bool DoesHaveAlignmentTag = Context.DoesArchetypeHaveTag<FFlockAlignmentTag>();
		const bool DoesHaveAvoidanceTag = Context.DoesArchetypeHaveTag<FFlockAvoidanceTag>();
					
		const FFlockCohesionParameters* CohesionParameters = Context.GetConstSharedFragmentPtr<FFlockCohesionParameters>();
		const FFlockAlignmentParameters* AlignmentParameters = Context.GetConstSharedFragmentPtr<FFlockAlignmentParameters>();
		const FFlockAvoidanceParameters* AvoidanceParameters = Context.GetConstSharedFragmentPtr<FFlockAvoidanceParameters>();
		
		const FNavigationObstacleHashGrid2D& HashGrid = NavigationSubsystem->GetObstacleGridMutable();
		const TConstArrayView<FTransformFragment> TransformFragments = Context.GetFragmentView<FTransformFragment>();
		const TArrayView<FMassForceFragment> ForceFragments = Context.GetMutableFragmentView<FMassForceFragment>();

		FVector CohesionForce = FVector::ZeroVector; 
		FVector AlignmentForce = FVector::ZeroVector;
		FVector AvoidanceForce = FVector::ZeroVector;
		
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			const FTransform Transform = TransformFragments[EntityIndex].GetTransform();
			const FVector Location = Transform.GetLocation();
			
			FVector& Force = ForceFragments[EntityIndex].Value;

			// Find neighbours
			const float MaxRadius = GetMaxRadius(Context);
			const int MaxResults = GetMaxNeighbours(Context);
			TArray<FMassNavigationObstacleItem, TFixedAllocator<21>> NearbyEntities;
			UE::MassFlock::Utilities::FindCloseObstacles(Location, MaxRadius, HashGrid, NearbyEntities, MaxResults);

			CohesionForce.Zero(); AlignmentForce.Zero(); AvoidanceForce.Zero();
			int CohesionCount = 0, AlignmentCount = 0, AvoidanceCount = 0;

			for (int j = 0; j < NearbyEntities.Num(); j++)
			{
				const auto NeighbourEntity = NearbyEntities[j];

				if (NeighbourEntity.Entity == Context.GetEntity(EntityIndex))
					continue;
							
				if (EntityManager.IsEntityValid(NeighbourEntity.Entity) == false)
					continue;
				
				FMassEntityView EntityView(EntityManager, NeighbourEntity.Entity);
				const FTransformFragment& NearbyTransform = EntityView.GetFragmentData<FTransformFragment>();

				const FVector OffsetFromNeighbour = NearbyTransform.GetTransform().GetLocation() - Location;
				const float SquaredDistance = OffsetFromNeighbour.SizeSquared2D(); 

				// Cohesion
				if (DoesHaveCohesionTag && SquaredDistance < CohesionParameters->SquaredRadius())
				{
					const float T = SquaredDistance / CohesionParameters->SquaredRadius(); // 1.0f when we're far, 0.0f when we're close
					const FVector Direction = OffsetFromNeighbour.GetSafeNormal2D();  // Towards neighbour
					CohesionForce += T * Direction * CohesionParameters->Force;
					CohesionCount++;
				}

				// Alignment
				if (DoesHaveAlignmentTag && SquaredDistance < AlignmentParameters->SquaredRadius())
				{
					const FMassVelocityFragment& NearbyVelocity = EntityView.GetFragmentData<FMassVelocityFragment>();
					AlignmentForce += FVector(NearbyVelocity.Value.X, NearbyVelocity.Value.Y, 0); // Towards same target as neighbour
					AlignmentCount++;
				}

				// Avoidance
				if (DoesHaveAvoidanceTag && SquaredDistance < AvoidanceParameters->SquaredRadius())
				{
					const float T = 1.0f - SquaredDistance / AvoidanceParameters->SquaredRadius(); // 0.0f when we're far, 1.0f when we're close
					const FVector Direction = -OffsetFromNeighbour.GetSafeNormal2D(); // Away from neighbour
					AvoidanceForce += T * Direction * AvoidanceParameters->Force;
					AvoidanceCount++;
				}
			}

			if (CohesionCount > 0)
            {
				CohesionForce = CohesionForce / CohesionCount;
				Force += CohesionForce;
            }
            
            if (AlignmentCount > 0)
            {
            	AlignmentForce /= AlignmentCount;
            	Force += AlignmentForce.GetSafeNormal() * AlignmentParameters->Force;
            }
            
            if (AvoidanceCount > 0)
            {
            	AvoidanceForce = AvoidanceForce / AvoidanceCount;
            	Force += AvoidanceForce;
            }
		}
	});
}

float UFlockProcessor::GetMaxRadius(FMassExecutionContext& Context) const
{
	const FFlockCohesionParameters* CohesionParameters = Context.GetConstSharedFragmentPtr<FFlockCohesionParameters>();
	const FFlockAlignmentParameters* AlignmentParameters = Context.GetConstSharedFragmentPtr<FFlockAlignmentParameters>();
	const FFlockAvoidanceParameters* AvoidanceParameters = Context.GetConstSharedFragmentPtr<FFlockAvoidanceParameters>();

	const float CohesionRadius = (CohesionParameters) ? (CohesionParameters->Radius) : (0.0f);
	const float AlignmentRadius = (AlignmentParameters) ? (AlignmentParameters->Radius) : (0.0f);
	const float AvoidanceRadius = (AvoidanceParameters) ? (AvoidanceParameters->Radius) : (0.0f);

	return FMath::Max3(CohesionRadius, AlignmentRadius, AvoidanceRadius);
}

int UFlockProcessor::GetMaxNeighbours(FMassExecutionContext& Context) const
{
	const FFlockCohesionParameters* CohesionParameters = Context.GetConstSharedFragmentPtr<FFlockCohesionParameters>();
	const FFlockAlignmentParameters* AlignmentParameters = Context.GetConstSharedFragmentPtr<FFlockAlignmentParameters>();
	const FFlockAvoidanceParameters* AvoidanceParameters = Context.GetConstSharedFragmentPtr<FFlockAvoidanceParameters>();

	const int CohesionCount = (CohesionParameters) ? (CohesionParameters->MaxNeighbours) : (0);
	const int AlignmentCount = (AlignmentParameters) ? (AlignmentParameters->MaxNeighbours) : (0);
	const int AvoidanceCount = (AvoidanceParameters) ? (AvoidanceParameters->MaxNeighbours) : (0);

	return FMath::Max3(CohesionCount, AlignmentCount, AvoidanceCount);
}