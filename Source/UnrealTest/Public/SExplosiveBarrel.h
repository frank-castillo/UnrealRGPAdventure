// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class URadialForceComponent;

UCLASS()
class UNREALTEST_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* StaticMeshComp;

    UPROPERTY(BlueprintReadOnly, EditAnywhere )
    URadialForceComponent* RadialForceComp;

    virtual void PostInitializeComponents() override;
    
    // Must be marked with this in order to bind it with events
    UFUNCTION()
    void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
