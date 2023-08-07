// Fill out your copyright notice in the Description page of Project Settings.

#include "SDashProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASDashProjectile::ASDashProjectile()
{
    TeleportDelay = 0.2f;
    DetonateDelay = 0.2f;

    MoveComp->InitialSpeed = 6000.0f;
}

// Called when the game starts or when spawned
void ASDashProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Get the timer so we can get the initial trigger time of the handle and then wait the DetonateDelay time to trigger the function Explode
    // Like yield new WaitForSeconds in Unity
    GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDashProjectile::Explode, DetonateDelay);
}

void ASDashProjectile::Explode_Implementation()
{
    // Clear timer if the explode was already called through another source like OnActorHit
    GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

    UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

    // Stop the particle effect from playing back, "Erasing it" from the world
    EffectComp->DeactivateSystem();

    // Stop the movement and freeze in place
    // Then we set it to stop colliding so it wont stay as an invisible collider in the world
    MoveComp->StopMovementImmediately();
    SetActorEnableCollision(false);

    FTimerHandle TimerHandle_DelayedTeleport;
    GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDashProjectile::TeleportInstigator, TeleportDelay);

    // Skip the base implementation as we do not want to destroy the projectile actor, we need to keep it alive so we can teleport the instigator
    //Super::Explode_Implementation();
}

void ASDashProjectile::TeleportInstigator()
{
    AActor* ActorToTeleport = GetInstigator();

    // Make sure the pointer is not null
    if (ensure(ActorToTeleport))
    {
        // We need to keep instigator's rotation or it may end up jarring
        // The last false is used to prevent from checking if it will spawn inside any other object, if true, it will offset the actor as to not cause issues with physics
        // Usually set to false so it can actually check for this behaviour
        ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);
    }
}