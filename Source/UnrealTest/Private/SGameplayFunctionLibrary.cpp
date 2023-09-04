// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"
#include "SAttributeComponent.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
    USAttributeComponent* AttributeComponent = USAttributeComponent::GetAttributes(TargetActor);

    if (AttributeComponent)
    {
        return AttributeComponent->ApplyHealthChange(DamageCauser, -DamageAmount);
    }

    return false;

}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
    if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
    {
        UPrimitiveComponent* HitComp = HitResult.GetComponent();

        if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
        {
            // Impulse -> Direction and Magnitude
            // Normal is the direction back to who shot
            // To get the origin of where we are shooting figure the trace start, trace end and turn it into a vector
            FVector Direction = HitResult.TraceEnd - HitResult.TraceStart; // Direction is Target - Origin
            Direction.Normalize(); // Properly turn to direction vector

            HitComp->AddImpulseAtLocation(Direction * 300000.0f, HitResult.ImpactPoint, HitResult.BoneName);
        }

        return true;
    }

    return false;
}