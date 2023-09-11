// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "Components/SphereComponent.h"
#include <Components/StaticMeshComponent.h>
#include <Net/UnrealNetwork.h>

// Sets default values
ASPowerupActor::ASPowerupActor()
{
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SphereComp->SetCollisionProfileName(TEXT("Powerup"));
    RootComponent = SphereComp;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));

    // Disable collision, instead we will use SphereComp to handle interaction queries
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetupAttachment(RootComponent);

    RespawnTime = 10.0f;
    bIsActive = true;

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
    GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPowerupActor::ShowPowerup, RespawnTime);
}

void ASPowerupActor::SetPowerupState(bool bNewIsActive)
{
    // Before multiplayer
    //SetActorEnableCollision(bNewIsActive);

    // Set visibility on root and all children
    //RootComponent->SetVisibility(bNewIsActive, true);

    // After Multiplayer
    bIsActive = bNewIsActive;
    OnRep_IsActive(); // Call the Rep so server can update local Actor
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASPowerupActor, bIsActive);
}

void ASPowerupActor::OnRep_IsActive()
{
    SetActorEnableCollision(bIsActive);

    // Set visibility on root and all children
    RootComponent->SetVisibility(bIsActive, true);
}
