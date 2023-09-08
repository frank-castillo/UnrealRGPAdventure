// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraShakeBase.h"

// Sets default values
ASProjectileBase::ASProjectileBase()
{
    SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
    // Other ways to set up collision detection and type of object
    // However, best to set a profile and let it handle the rest
    //SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
    //SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    //SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SphereComp->SetCollisionProfileName("Projectile");
    RootComponent = SphereComp;

    EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
    EffectComp->SetupAttachment(RootComponent);

    AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
    AudioComp->SetupAttachment(RootComponent);

    MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMoveComp");
    MoveComp->bRotationFollowsVelocity = true;
    MoveComp->bInitialVelocityInLocalSpace = true;
    MoveComp->ProjectileGravityScale = 0.0;
    MoveComp->InitialSpeed = 8000.0f;

    ImpactShakeInnerRadius = 250.0f;
    ImpactShakeOuterRadius = 2500.0f;

    // AI needs it to replicate
    bReplicates = true;
}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Explode();
}


// We need the _Implementation from it being marked as BlueprintNativeEvent so C++ can detect the implementation
// The function however will still be called by its regular method name such as OnActorHit
void ASProjectileBase::Explode_Implementation()
{
    // We need to make sure to check we are not already being destroyed
    // With ensure we make sure we stall the implementation and debug to see why this is being triggered
    if (ensure(!IsPendingKill()))
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
        UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);

        //EffectComp->DeactivateSystem();

        //MoveComp->StopMovementImmediately();
        //SetActorEnableCollision(false);

        Destroy();
    }
}

void ASProjectileBase::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
    //SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}
