// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Coin.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include <Components/StaticMeshComponent.h>

ASPowerup_Coin::ASPowerup_Coin()
{
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));

    // Disable collision, instead we will use SphereComp to handle interaction queries
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetupAttachment(RootComponent);

    CreditsToGrant = 5;
}

void ASPowerup_Coin::Interact_Implementation(APawn* InstigatorPawn)
{
    if (!ensure(InstigatorPawn))
    {
        return;
    }

    if(ASCharacter* Character = Cast<ASCharacter>(InstigatorPawn))
    {
        ASPlayerState* PlayerState = CastChecked<ASPlayerState>(Character->GetPlayerState());

        // Only activate if healed successfully
        if (PlayerState->GrantCredits(CreditsToGrant))
        {
            HideAndCooldownPowerup();
        }
    }
}