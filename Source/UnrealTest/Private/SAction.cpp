// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"

bool USAction::CanStart_Implementation(AActor* Instigator)
{
    // Prevent spam clicking
    if (IsRunning())
    {
        return false;
    }

    // Check for blocking tags
    USActionComponent* Comp = GetOwningComponent();

    if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
    {
        return false;
    }

    return true;
}

void USAction::StartAction_Implementation(AActor* Instigator)
{
    UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

    // We need to use the outer so we have the actual ActionComponent the action belongs to
    USActionComponent* Comp = GetOwningComponent();
    Comp->ActiveGameplayTags.AppendTags(GrantsTags);

    bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
    UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));

    ensureAlways(bIsRunning); // Safety check to ensure we can test proper bugs and see if we made a mistake on the logic when this should not be triggering at all

    USActionComponent* Comp = GetOwningComponent();
    Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

    bIsRunning = false;
}

bool USAction::IsRunning() const
{
    return bIsRunning;
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

USActionComponent* USAction::GetOwningComponent() const
{
    return Cast<USActionComponent>(GetOuter());
}
