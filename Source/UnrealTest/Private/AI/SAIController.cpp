// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include <Kismet/GameplayStatics.h>
#include <BehaviorTree/BlackboardComponent.h>

void ASAIController::BeginPlay()
{
    Super::BeginPlay();

    if (ensureMsgf(BehaviourTree, TEXT("Behaviour Tree is nullptr. Please assign behaviour tree in your AI Controller.")))
    {
        RunBehaviorTree(BehaviourTree);
    }

    // Previous implementation used when there was no sense of sight
    // We would get the player from the dictionary of objects registered to the game settings and assign from the get go
    /*APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    if (MyPawn)
    {
        GetBlackboardComponent()->SetValueAsVector(TEXT("MoveToLocation"), MyPawn->GetActorLocation());

        GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), MyPawn);
    }*/
}
