// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Coin.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include <Components/StaticMeshComponent.h>

ASPowerup_Coin::ASPowerup_Coin()
{
    CreditsToGrant = 5;
}

void ASPowerup_Coin::Interact_Implementation(APawn* InstigatorPawn)
{
    if (!ensure(InstigatorPawn))
    {
        return;
    }

    // Looman implementation
    if (ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>())
    {
        PlayerState->AddCredits(CreditsToGrant);
        HideAndCooldownPowerup();
    }

    /*
    My Own Implementation
    if(ASCharacter* Character = Cast<ASCharacter>(InstigatorPawn))
    {
        ASPlayerState* PlayerState = CastChecked<ASPlayerState>(Character->GetPlayerState());

        // Only activate if healed successfully
        if (PlayerState->GrantCredits(CreditsToGrant))
        {
            HideAndCooldownPowerup();
        }
    }
    */
}