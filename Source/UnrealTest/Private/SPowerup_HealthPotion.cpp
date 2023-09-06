// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"

// Sets default values
ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    
    // Disable collision, instead we will use SphereComp to handle interaction queries
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetupAttachment(RootComponent);

    CostToUsePotion = 50;
}

void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
    if (!ensure(InstigatorPawn))
    {
        return;
    }

    // Looman Implementation
    USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);

    if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
    {
        if (ASPlayerState* PS = InstigatorPawn->GetPlayerState<ASPlayerState>())
        {
            if (PS->RemoveCredits(CostToUsePotion) && AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthMax()))
            {
                // Only trigger if healed successfully
                HideAndCooldownPowerup();
            }
        }
    }

    // Own Implementation
    /*
    if (ASCharacter* Character = Cast<ASCharacter>(InstigatorPawn))
    {
        ASPlayerState* PlayerState = Cast<ASPlayerState>(Character->GetPlayerState());

        if (!PlayerState)
        {
            return;
        }

        float Credits = PlayerState->GetCredits();

        if (Credits >= CostToUsePotion)
        {
            USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);

            // Check if it exists and not already at max health
            if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
            {
                // Only activate if healed successfully
                if (AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthMax()))
                {
                    PlayerState->DeductCredits(CostToUsePotion);
                    HideAndCooldownPowerup();
                }
            }
        }
    }
    */
}