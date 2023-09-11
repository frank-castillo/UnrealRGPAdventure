// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SPowerupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class UNREALTEST_API ASPowerupActor : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void ShowPowerup();

	void HideAndCooldownPowerup();

	void SetPowerupState(bool bNewIsActive);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, Category = "Powerup")
	float RespawnTime;

    FTimerHandle TimerHandle_RespawnTimer;

    UPROPERTY(ReplicatedUsing="OnRep_IsActive")
    bool bIsActive;

    UFUNCTION()
    void OnRep_IsActive();

public:

	// Sets default values for this actor's properties
	ASPowerupActor();


	void Interact_Implementation(APawn* InstigatorPawn) override;
};
