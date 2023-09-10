// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"
#include "../UnrealTest.h"
#include <Net/UnrealNetwork.h>
#include <GameFramework/Actor.h>

void USAction::Initialize(USActionComponent* NewActionComp)
{
    ActionComp = NewActionComp;
}

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
    //UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
    LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

    // We need to use the outer so we have the actual ActionComponent the action belongs to
    USActionComponent* Comp = GetOwningComponent();
    Comp->ActiveGameplayTags.AppendTags(GrantsTags);

    bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
    //UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
    LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

    // Safety check to ensure we can test proper bugs and see if we made a mistake on the logic when this should not be triggering at all
    // Removed as it would only work on the server
    //ensureAlways(bIsRunning); 

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
    
    // Used for Unreal 4.27 as it changed the outer by itself
    //AActor* Actor = Cast<AActor>(GetOuter());
    //if (Actor)
    //{
    //    return Actor->GetWorld();
    //}

    // In Unreal 5.0+ this has been fixed to stay as the outer, even on network
    UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
    if (Comp)
    {
        return Comp->GetWorld();
    }

    return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
    // Old implementation - Changed as now Unreal on the network transforms our Outer to an Actor owning the component, so we have to redo this to match Unreal
    // However, it seems that in in Unreal 5.0+ this has been fixed and now the component stays as the default outer
    return Cast<USActionComponent>(GetOuter());

    // Not optimized at all!
    //AActor* Actor = Cast<AActor>(GetOuter());
    //return Actor->GetComponentByClass(USActionComponent::StaticClass());

    // New implementation
    //return ActionComp;
}

// Triggered when bIsRunning gets updated on the network, allowing to play this on Server and Clients
void USAction::OnRep_IsRunning()
{
    if (bIsRunning)
    {
        StartAction(nullptr);
    }
    else
    {
        StopAction(nullptr);
    }
}

void USAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(USAction, bIsRunning);
    DOREPLIFETIME(USAction, ActionComp);
}