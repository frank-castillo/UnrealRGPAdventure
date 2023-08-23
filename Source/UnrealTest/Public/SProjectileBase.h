// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UAudioComponent;
class USoundCue;
class UCameraShakeBase;

// Abstract marks the class as incomplete, keeping this out of certain dropdown windows
UCLASS(ABSTRACT)
class UNREALTEST_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeOuterRadius;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* MoveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* SphereComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UParticleSystemComponent* EffectComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* ImpactVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* ImpactSound;

    UPROPERTY(VisibleAnywhere, Category = "Components")
	UAudioComponent* AudioComp;

    // Virtual so we can override on child classes
    UFUNCTION()
    virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // Blueprint NativeEvent = C++ base implementation, can be expanded in Blueprints
    // BlueprintCallable to allow child classes to trigger explosions
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Explode();

    virtual void PostInitializeComponents() override;
};
