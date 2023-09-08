// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"
#include "SAction.h"

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

    USAction* NewAction = NewObject<USAction>(this, ActionClass);

    if (ensure(NewAction))
    {
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

            // Is client?
            // This sends the data to the server
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
                Action->StopAction(Instigator);
                return true;
            }
        }
    }

    return false;
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

    for (TSubclassOf<USAction> ActionClass : DefaultActions)
    {
        AddAction(GetOwner(), ActionClass);
    }
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // GEngine is a global pointer to our engine, accessible from anywhere
    FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple(); // We use simple, as it has the least amount of markup language
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
    StartActionByName(Instigator, ActionName);
}