// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SMagicProjectile.generated.h"

UCLASS()
class UNREALTEST_API ASMagicProjectile : public ASProjectileBase // Refactor from base class
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASMagicProjectile();

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float DamageAmount;

    // We dive into the implementation of the original component overlap
    // We copy the arguments the function requires
    // We add UFUNCTION! So we can properly bind the function
    UFUNCTION()
    void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    void PostInitializeComponents() override;
};
