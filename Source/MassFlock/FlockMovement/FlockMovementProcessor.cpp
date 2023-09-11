// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockMovementProcessor.h"

#include "FlockMovementTrait.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassMovementFragments.h"
#include "MassSimulationLOD.h"
#include "Kismet/KismetMathLibrary.h"

UFlockMovementProcessor::UFlockMovementProcessor()
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
}

void UFlockMovementProcessor::ConfigureQueries()
{
	EntityQuery.AddConstSharedRequirement<FFlockMovementParameters>();
	
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassForceFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);

	EntityQuery.AddRequirement<FMassSimulationVariableTickFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddChunkRequirement<FMassSimulationVariableTickChunkFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.SetChunkFilter(&FMassSimulationVariableTickChunkFragment::ShouldTickChunkThisFrame);
	
	EntityQuery.RegisterWithProcessor(*this);
}

void UFlockMovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([this](FMassExecutionContext& Context)
	{
		const FFlockMovementParameters MovementParameters = Context.GetConstSharedFragment<FFlockMovementParameters>();
		
		const TArrayView<FTransformFragment> TransformFragments = Context.GetMutableFragmentView<FTransformFragment>();
		const TArrayView<FMassForceFragment> ForceFragments = Context.GetMutableFragmentView<FMassForceFragment>();
		const TArrayView<FMassVelocityFragment> VelocityFragments = Context.GetMutableFragmentView<FMassVelocityFragment>();
		const TConstArrayView<FMassSimulationVariableTickFragment> SimVariableTickList = Context.GetFragmentView<FMassSimulationVariableTickFragment>();
		const bool bHasVariableTick = (SimVariableTickList.Num() > 0);
		const float WorldDeltaTime = Context.GetDeltaTimeSeconds();
		
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			const float DeltaTime = bHasVariableTick ? SimVariableTickList[EntityIndex].DeltaTime : WorldDeltaTime;

			// Apply current forces to velocity
			FVector& Velocity = VelocityFragments[EntityIndex].Value;
			FVector& Force = ForceFragments[EntityIndex].Value;
			Velocity += Force * DeltaTime;
			Force = FVector::ZeroVector;

			// Clamp velocity
			if (Velocity.SquaredLength() > MovementParameters.MaxSpeedSquared())
			{
				Velocity = Velocity.GetSafeNormal() * MovementParameters.MaxSpeed;
			}

			// Movement
			FTransform& Transform = TransformFragments[EntityIndex].GetMutableTransform();
			Transform.SetTranslation(Transform.GetTranslation() + Velocity * DeltaTime);

			// Rotation
			if (MovementParameters.RotateTowardsMovement)
			{
				const FVector Location = Transform.GetLocation();
				FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + Velocity);
				Transform.SetRotation(Rotator.Quaternion());
			}
		}
	}));
}


