// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"
#include "SAction.h"
#include "../UnrealTest.h"
#include <Net/UnrealNetwork.h>
#include <Engine/ActorChannel.h>

USActionComponent::USActionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    SetIsReplicatedByDefault(true);
}

void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
    if (!ensure(ActionClass))
    {
        return;
    }

    // Skip for clients
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("Client attempting to AddAction! [Class: %s]"), *GetNameSafe(ActionClass));
        return;
    }

    // Unreal Engine 4.27 implementation
    //USAction* NewAction = NewObject<USAction>(GetOwner(), ActionClass);

    // Unreal Engine 5.0+ implementation
    USAction* NewAction = NewObject<USAction>(this, ActionClass);
    if (ensure(NewAction))
    {
        NewAction->Initialize(this); // Fix the network discrepancy between server and client
        Actions.Add(NewAction);

        if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
        {
            NewAction->StartAction(Instigator);
        }
    }
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
    if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
    {
        return;
    }

    Actions.Remove(ActionToRemove);
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
    for (USAction* Action : Actions)
    {
        if (Action && Action->ActionName == ActionName)
        {
            if (!Action->CanStart(Instigator))
            {
                FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
                // Check the other actions
                continue;
            }

            // Is this a client?
            // IF SO, this sends the data to the server to trigger the action for both the server and client
            // The server gets triggered by this ServerStartAction function, while the client gets triggered on the call after the if statement
            if (!GetOwner()->HasAuthority())
            {
                ServerStartAction(Instigator, ActionName);
            }

            Action->StartAction(Instigator);
            return true;
        }
    }

    return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
    for (USAction* Action : Actions)
    {
        if (Action && Action->ActionName == ActionName)
        {
            if (Action->IsRunning())
            {
                // Is this a client?
                // IF SO, this sends the data to the server to trigger the action for both the server and client
                // The server gets triggered by this ServerStartAction function, while the client gets triggered on the call after the if statement
                if (!GetOwner()->HasAuthority())
                {
                    ServerStopAction(Instigator, ActionName);
                }

                Action->StopAction(Instigator);
                return true;
            }
        }
    }

    return false;
}

USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{
    for (USAction* Action : Actions)
    {
        if (Action && Action->IsA(ActionClass))
        {
            return Action;
        }
    }

    return nullptr;
}

void USActionComponent::BeginPlay()
{
    Super::BeginPlay();

    // Server ONLY! -> without replication, only the server has actions...
    if (GetOwner()->HasAuthority())
    {
        for (TSubclassOf<USAction> ActionClass : DefaultActions)
        {
            AddAction(GetOwner(), ActionClass);
        }
    }
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // GEngine is a global pointer to our engine, accessible from anywhere
    //FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple(); // We use simple, as it has the least amount of markup language
    //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);

    // Draw all actions
    for (USAction* Action : Actions)
    {
        FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;

        // Unreal Engine 4.27
        /*FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s : IsRunning: %s : Outer: %s"),
            *GetNameSafe(GetOwner()),
            *Action->ActionName.ToString(),
            Action->IsRunning() ? TEXT("true") : TEXT("false"),
            *GetNameSafe(Action->GetOuter()));*/

            // Unreal Engine 5.0+
            /*FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s : IsRunning: %s : Outer: %s"),
                *GetNameSafe(GetOwner()),
                *Action->ActionName.ToString(),
                Action->IsRunning() ? TEXT("true") : TEXT("false"),
                *GetNameSafe(GetOuter()));*/

                // Less intrusive Debug
        FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s : IsRunning: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));

        LogOnScreen(this, ActionMsg, TextColor, 0.0f);
    }
}

// Network

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
    StartActionByName(Instigator, ActionName);
}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
    StopActionByName(Instigator, ActionName);
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(USActionComponent, Actions);
}

bool USActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (USAction* Action : Actions)
    {
        if (Action)
        {
            // Bitwise or should increase performance in this section
            WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
        }
    }

    return WroteSomething;
}