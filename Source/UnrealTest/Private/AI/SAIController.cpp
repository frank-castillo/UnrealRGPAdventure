// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include <Kismet/GameplayStatics.h>
#include <BehaviorTree/BlackboardComponent.h>

void ASAIController::BeginPlay()
{
    Super::BeginPlay();

    RunBehaviorTree(BehaviourTree);

    APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    if (MyPawn)
    {
        GetBlackboardComponent()->SetValueAsVector(TEXT("MoveToLocation"), MyPawn->GetActorLocation());

        GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), MyPawn);
    }
}
