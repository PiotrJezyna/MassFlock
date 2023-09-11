// Copyright Piotr Jeżyna. All Rights Reserved.


#include "FlockCohesionTrait.h"

#include "MassEntityTemplateRegistry.h"

void UFlockCohesionTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddTag<FFlockCohesionTag>();
	
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	const FConstSharedStruct SharedParameters = EntityManager.GetOrCreateConstSharedFragment(Parameters);
	BuildContext.AddConstSharedFragment(SharedParameters);
}
