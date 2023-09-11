// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class UNREALTEST_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASItemChest();

    void Interact_Implementation(APawn* InstigatorPawn);

    void OnActorLoaded_Implementation();

    UPROPERTY(EditAnywhere)
    double TargetPitch;

protected:

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BaseMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* LidMesh;

    UPROPERTY(ReplicatedUsing = "OnRep_LidOpened", BlueprintReadOnly, SaveGame) // RepNotify
    bool bLidOpened;

    UFUNCTION()
    void OnRep_LidOpened();
};
