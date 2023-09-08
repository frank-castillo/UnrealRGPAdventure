// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "Components/SphereComponent.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SphereComp->SetCollisionProfileName(TEXT("Powerup"));
    RootComponent = SphereComp;

    RespawnTime = 10.0f;

    bReplicates = true;
}

void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn)
{
    // Logic in derived classes
}

void ASPowerupActor::ShowPowerup()
{
    SetPowerupState(true);
}

void ASPowerupActor::HideAndCooldownPowerup()
{
    SetPowerupState(false);

    FTimerHandle TimerHandle_RespawnTimer;
    GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPowerupActor::ShowPowerup, RespawnTime);
}

void ASPowerupActor::SetPowerupState(bool bNewIsActive)
{
    SetActorEnableCollision(bNewIsActive);

    // Set visibility on root and all children
    RootComponent->SetVisibility(bNewIsActive, true);
}