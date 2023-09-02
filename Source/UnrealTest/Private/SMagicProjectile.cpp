// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "SGameplayFunctionLibrary.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
    SphereComp->SetSphereRadius(20.0f);
    SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);

    DamageAmount = 20.0f;
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != GetInstigator())
    {
        // We use StaticClass in order to suffice the argument requirement needed for the Get function
        // We get the component so that we can then access its functions and members
        //USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));

        //if (AttributeComp)
        //{
        //    // Reduce Player Health
        //    AttributeComp->ApplyHealthChange(GetInstigator(), -DamageAmount);

        //    // Only explode when we hit something valid
        //    Explode();
        //}

        if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
        {
            Explode();
        }
    }
}

void ASMagicProjectile::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    //SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}
