// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SActionEffect_Thorns.h"
#include "SAttributeComponent.h"
#include "SActionComponent.h"
#include "SGameplayFunctionLibrary.h"

USActionEffect_Thorns::USActionEffect_Thorns()
{
    ReflectFraction = 0.2f;

    Duration = 0.0f;
    Period = 0.0f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
    Super::StartAction_Implementation(Instigator);

    // Start listening for health change value
    USAttributeComponent* Attributes = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());

    if (Attributes)
    {
        Attributes->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
    }
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
    Super::StopAction_Implementation(Instigator);

    // Start listening for health change value
    // Remember to include the Action component so we can access the owner of the actions
    USAttributeComponent* Attributes = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());

    if (Attributes)
    {
        Attributes->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
    }
}

void USActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    AActor* OwningActor = GetOwningComponent()->GetOwner();

    // Damage Only
    if (Delta < 0.0f && OwningActor != InstigatorActor)
    {
        // Round to nearest whole value to avoid weird health values and partitions
        int32 ReflectedAmount = FMath::RoundToInt(Delta * ReflectFraction);

        if (ReflectedAmount == 0)
        {
            // Early return as there was no damage to bounce back
            return;
        }

        // Flip to positive, so we don't heal target when dealing damage
        ReflectedAmount = FMath::Abs(ReflectedAmount);

        USGameplayFunctionLibrary::ApplyDamage(OwningActor, InstigatorActor, ReflectedAmount);
    }
}