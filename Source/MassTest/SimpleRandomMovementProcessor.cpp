// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRandomMovementProcessor.h"
#include "SimpleMovementFragment.h"
#include "MassCommonTypes.h"
#include "MassRequirements.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"

USimpleRandomMovementProcessor::USimpleRandomMovementProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    ExecutionFlags = (int32)EProcessorExecutionFlags::All;
    ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void USimpleRandomMovementProcessor::ConfigureQueries()
{
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FSimpleMovementFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.RegisterWithProcessor(*this);
}

void USimpleRandomMovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    EntityQuery.ForEachEntityChunk(EntityManager, Context, ([this](FMassExecutionContext& Context)
    {
        const TArrayView<FTransformFragment> TransformsList = Context.GetMutableFragmentView<FTransformFragment>();
        const TArrayView<FSimpleMovementFragment> SimpleMovementsList = Context.GetMutableFragmentView<FSimpleMovementFragment>();
        const float WorldDeltaTime = Context.GetDeltaTimeSeconds();

        for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
        {
            FTransform& Transform = TransformsList[EntityIndex].GetMutableTransform();
            FVector& MoveTarget = SimpleMovementsList[EntityIndex].Target;

            FVector CurrentLocation = Transform.GetLocation();
            FVector TargetVector = MoveTarget - CurrentLocation;

            if (TargetVector.Size() <= 20.f)
            {
                MoveTarget = FVector(FMath::RandRange(-1.f, 1.f) * 1000.f, FMath::RandRange(-1.f, 1.f) * 1000.f, CurrentLocation.Z);
            }
            else
            {
                Transform.SetLocation(CurrentLocation + TargetVector.GetSafeNormal() * 400.f * WorldDeltaTime);
            }
        }
    }));
}