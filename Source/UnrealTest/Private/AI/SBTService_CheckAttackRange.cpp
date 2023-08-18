// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include <AIModule/Classes/AIController.h>

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // Check distance between AI pawn and targetActor
    UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();

    if (ensure(BlackBoardComp))
    {
        AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject("TargetActor"));

        if (TargetActor)
        {
            AAIController* MyController = OwnerComp.GetAIOwner();

            if (ensure(MyController))
            {
                APawn* AIPawn = MyController->GetPawn();

                if (ensure(AIPawn))
                {
                    float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

                    bool bWithinRange = DistanceTo < 2000.f;

                    // Line of sight is already part of the AIController
                    bool bHasLineOfSight = false;
                    
                    if (bWithinRange)
                    {
                        bHasLineOfSight = MyController->LineOfSightTo(TargetActor);
                    }

                    // With the predefined blackboard variable, we can automatically get the name or value of the key and use it with the blackboard's functions
                    BlackBoardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLineOfSight));
                }
            }
        }
    }
}
