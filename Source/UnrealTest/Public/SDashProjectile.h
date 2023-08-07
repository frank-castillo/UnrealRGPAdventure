// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SprojectileBase.h"
#include "SDashProjectile.generated.h"

UCLASS()
class UNREALTEST_API ASDashProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDashProjectile();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	// Handle in order to cancel the movement if we collide with something in the world
	FTimerHandle TimerHandle_DelayedDetonate;

	// Base class using BlueprintNativeEvent, we must override the _Implementation not the Explode() itself
	// This is so Unreal will detect the C++ implementation
	virtual void Explode_Implementation() override;

	void TeleportInstigator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
