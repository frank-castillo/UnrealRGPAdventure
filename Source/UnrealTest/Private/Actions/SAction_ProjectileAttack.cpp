// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction_ProjectileAttack.h"
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
    HandSocketName = "Muzzle_01";
    AttackAnimDelay = 0.2f;
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
    // Always check to make sure we are passing the right class and the argument is not null
    if (ensureAlways(ProjectileClass))
    {
        // Hash string method to find the socket name
        FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Instigator = InstigatorCharacter;

        // Debug
        FCollisionShape Shape;
        Shape.SetSphere(20.0f);

        // Ignore Player
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(InstigatorCharacter);

        // Set up collision layers
        FCollisionObjectQueryParams CollisionObjectParams;
        CollisionObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
        CollisionObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
        CollisionObjectParams.AddObjectTypesToQuery(ECC_Pawn);

        // Begin the ray trace to line up projectile with crosshair, regardless if avatar is left or right side oriented
        // First get camera position
        // Ignore this line of code as it is the previous implementation used directly in character
        //FVector TraceStart = CameraComp->GetComponentLocation();
        // Instead we use the overridden method we created to 
        FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();

        // Then set the end point of the trace collision check. This is pointing towards the look at of the character.
        FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 5000);

        // Collision Logic - Check returns true if it collided with something
        FHitResult Hit;
        // Find new direction/rotation from Hand pointing direction to the impact point in the world
        FRotator ProjectionRotation;
        if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, CollisionObjectParams, Shape, CollisionParams))
        {
            // Adjust location to end up at crosshair look-at
            ProjectionRotation = FRotationMatrix::MakeFromX(Hit.ImpactPoint - HandLocation).Rotator();
        }
        else
        {
            ProjectionRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();
        }

        // Final Transform Matrix based on new X-aligned rotation and the hand position as origin
        FTransform SpawnTM = FTransform(ProjectionRotation, HandLocation);
        GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
    }

    StopAction(InstigatorCharacter);
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
    Super::StartAction_Implementation(Instigator);

    ACharacter* Character = Cast<ACharacter>(Instigator);

    if (Character)
    {
        // Play animation, spawn attached particle effect, set timer
        Character->PlayAnimMontage(AttackAnim);
        UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

        FTimerHandle TimerHandle_AttackDelay;
        FTimerDelegate Delegate;
        Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

        // This will call the function with the Character as the parameter being passed into the function
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
    }
}
