// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockBoundsTrait.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityUtils.h"
#include "MassMovementFragments.h"

void UFlockBoundsTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FTransformFragment>();
	BuildContext.RequireFragment<FMassForceFragment>();
	BuildContext.RequireFragment<FMassVelocityFragment>();
	
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	const FConstSharedStruct Params = EntityManager.GetOrCreateConstSharedFragment(Parameters);
	BuildContext.AddConstSharedFragment(Params);
}
