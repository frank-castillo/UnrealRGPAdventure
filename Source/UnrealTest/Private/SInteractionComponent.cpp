// Fill out your copyright notice in the Description page of Project Settings.

#include "SInteractionComponent.h"
#include "../Public/SGameplayInterface.h"
#include <DrawDebugHelpers.h>
#include "SWorldUserWidget.h"

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interact Component."), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    TraceRadius = 30.0f;
    TraceDistance = 500.0f;
    CollisionChannel = ECC_WorldDynamic;
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

    // Check who controls the InteractionComponent
    // If it is us, then just run my own logic and not the other clients as well on my own local version
    APawn* MyPawn = Cast<APawn>(GetOwner());
    if (MyPawn->IsLocallyControlled())
    {
        FindBestInteractable();
    }
}

void USInteractionComponent::FindBestInteractable()
{
    bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

    FVector EyeLocation;
    FRotator EyeRotation;
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

    AActor* MyOwner = GetOwner();
    MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance); // vector to know where we are pointing and then extending 1000 cm, so a meter

    // FHitResult Hit;
    // Raycast that hits the first thing it comes into contact with
    // bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

    TArray<FHitResult> Hits;
    FCollisionShape Shape;
    Shape.SetSphere(TraceRadius);

    bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

    FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

    // Clear ref before trying to fill
    FocusedActor = nullptr;

    for (FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor)
        {
            if (bDebugDraw)
            {
                //TODO: Fix the 0.0f back to 2.0f
                DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 5.0f, 0, 0.0f);
            }

            // Note to self:
            // Here, we use the refraction system of Unreal to reference the interface. We use UInterface because that is the reference UNreal compiler uses to then mirror the interface
            // Once we know the interface is there and exists, we can use the public mirrored interface class: IInterface to call the interface methods
            if (HitActor->Implements<USGameplayInterface>())
            {
                FocusedActor = HitActor;
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

    if (FocusedActor)
    {
        if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
        {
            DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
        }

        if (DefaultWidgetInstance)
        {
            DefaultWidgetInstance->AttachedActor = FocusedActor;

            if (!DefaultWidgetInstance->IsInViewport())
            {
                DefaultWidgetInstance->AddToViewport();
            }
        }
    }
    else
    {
        if (DefaultWidgetInstance)
        {
            DefaultWidgetInstance->RemoveFromParent(); // Same as remove from viewport
        }
    }

    // Debug Render
    if (bDebugDraw)
    {
        //TODO: Fix the 0.0f back to 2.0f
        DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 5.0f, 0, 0.0f);
    }
}

void USInteractionComponent::PrimaryIntetract()
{
    ServerInteract(FocusedActor);
}

void USInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
    if (InFocus == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact.");
        return;
    }

    APawn* MyPawn = Cast<APawn>(GetOwner());
    ISGameplayInterface::Execute_Interact(InFocus, MyPawn);
}