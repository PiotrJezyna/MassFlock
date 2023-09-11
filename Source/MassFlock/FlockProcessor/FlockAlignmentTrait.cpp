// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockAlignmentTrait.h"
#include "MassEntityTemplateRegistry.h"

void UFlockAlignmentTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddTag<FFlockAlignmentTag>();

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	FConstSharedStruct& SharedParameters = EntityManager.GetOrCreateConstSharedFragment(Parameters);
	BuildContext.AddConstSharedFragment(SharedParameters);
}
