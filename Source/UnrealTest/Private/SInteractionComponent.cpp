// Fill out your copyright notice in the Description page of Project Settings.

#include "SInteractionComponent.h"
#include "../Public/SGameplayInterface.h"
#include <DrawDebugHelpers.h>

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interact Component."), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

// Called when the game starts
void USInteractionComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
}

// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void USInteractionComponent::PrimaryIntetract()
{
    bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

    FVector EyeLocation;
    FRotator EyeRotation;
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    AActor* MyOwner = GetOwner();
    MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    FVector End = EyeLocation + (EyeRotation.Vector() * 1000); // vector to know where we are pointing and then extending 1000 cm, so a meter

    // FHitResult Hit;
    // Raycast that hits the first thing it comes into contact with
    // bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

    TArray<FHitResult> Hits;
    FCollisionShape Shape;
    float Radius = 30.0f;
    Shape.SetSphere(Radius);

    bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

    FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

    for (FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor)
        {
            if (bDebugDraw)
            {
                DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 5.0f, 0, 2.0f);
            }

            // Note to self:
            // Here, we use the refraction system of Unreal to reference the interface. We use UInterface because that is the reference UNreal compiler uses to then mirror the interface
            // Once we know the interface is there and exists, we can use the public mirrored interface class: IInterface to call the interface methods
            if (HitActor->Implements<USGameplayInterface>())
            {
                APawn* MyPawn = Cast<APawn>(MyOwner);
                ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
                break; // We found the object we were looking for
            }
        }
    }

    // Leave this out if using LineTrace
    //AActor* HitActor = Hit.GetActor();
    //if (HitActor)
    //{
    //    // Note to self:
    //    // Here, we use the refraction system of Unreal to reference the interface. We use UInterface because that is the reference UNreal compiler uses to then mirror the interface
    //    // Once we know the interface is there and exists, we can use the public mirrored interface class: IInterface to call the interface methods
    //    if (HitActor->Implements<USGameplayInterface>())
    //    {
    //        APawn* MyPawn = Cast<APawn>(MyOwner);
    //        ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
    //    }
    //}

    // Debug Render
    if (bDebugDraw)
    {
        DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 5.0f, 0, 2.0f);
    }
}