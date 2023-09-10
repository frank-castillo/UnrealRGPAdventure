// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Action.h"
#include "SActionComponent.h"
#include "SAction.h"

void ASPowerup_Action::Interact_Implementation(APawn* InstigatorPawn)
{
    // Ensure we have an instigator and the action class was set up
    if (!ensure(InstigatorPawn && ActionToGrant))
    {
        return;
    }

    USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));

    /// Check to see if player already has action class
    if (ActionComp)
    {
        if (ActionComp->GetAction(ActionToGrant))
        {
            FString DebugMsg = FString::Printf(TEXT("Action '%s' already known."), *GetNameSafe(ActionToGrant));
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
            return;
        }

        // Give new ability
        ActionComp->AddAction(InstigatorPawn, ActionToGrant);
        HideAndCooldownPowerup();
    }
}
