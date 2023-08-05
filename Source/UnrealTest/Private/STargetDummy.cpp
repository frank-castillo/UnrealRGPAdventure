// Fill out your copyright notice in the Description page of Project Settings.

#include "STargetDummy.h"
#include "Components/StaticMeshComponent.h"
#include "SAttributeComponent.h"

// Sets default values
ASTargetDummy::ASTargetDummy()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    AttributeComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

    // Trigger when health is changed (damage/healing)
    // Attribute component has the event that will be broadcast -> FOnHealthChanged
    // Here the Dummy subscribes its own function of OnHealthCHanged to that event that will be broadcast
    // Once the function of the attribute component gets triggered, we will receive an event here on the dummy and then the Dummy function will trigger
    // All of this gets triggered based on the Dynamic call being triggered by the MagicProjectile -> OnOverlap.AddDynamic(), get attribute component, and then apply the health change
    AttributeComponent->OnHealthChanged.AddDynamic(this, &ASTargetDummy::OnHealthChanged);
}

void ASTargetDummy::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.0f)
    {
        MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
    }
}