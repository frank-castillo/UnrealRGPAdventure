// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"
#include "SAttributeComponent.h"

// Sets default values
ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    
    // Disable collision, instead we will use SphereComp to handle interaction queries
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetupAttachment(RootComponent);
}

void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
    if (!ensure(InstigatorPawn))
    {
        return;
    }

    USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);

    // Check if it exists and not already at max health
    if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
    {
        // Only activate if healed successfully
        if (AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthMax()))
        {
            HideAndCooldownPowerup();
        }
    }
}