// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
    UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
    UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
}

UWorld* USAction::GetWorld() const
{
    // We want access to this in Blueprint, that is why we are overriding
    // This fixes problems later on when trying trace, sweeps, or world functions with actions
    // The outer is set when creating an action via NewObject() in the ActionComponent
    UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
    if (Comp)
    {
        return Comp->GetWorld();
    }

    return nullptr;
}
