// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockAvoidanceTrait.h"
#include "MassEntityTemplateRegistry.h"

void UFlockAvoidanceTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddTag<FFlockAvoidanceTag>();

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	FConstSharedStruct& SharedParameters = EntityManager.GetOrCreateConstSharedFragment(Parameters);
	BuildContext.AddConstSharedFragment(SharedParameters);
}
