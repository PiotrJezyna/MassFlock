// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockMovementTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "MassMovementFragments.h"

void UFlockMovementTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	FConstSharedStruct& SharedParameters = EntityManager.GetOrCreateConstSharedFragment(Parameters);
	BuildContext.AddConstSharedFragment(SharedParameters);
	
	BuildContext.RequireFragment<FMassVelocityFragment>();
	BuildContext.RequireFragment<FMassForceFragment>();
}
