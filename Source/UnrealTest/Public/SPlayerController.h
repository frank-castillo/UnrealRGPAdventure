// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

/**
 * 
 */
UCLASS()
class UNREALTEST_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

    UPROPERTY(BlueprintAssignable)
    FOnPawnChanged OnPawnChanged;

    // Listen for incoming player state (for clients when they join this might be a nullptr when joining a game.
    // afterwards player state will not change again as PlayerControllers maintain the same state throughout the level)
    UPROPERTY(BlueprintAssignable)
    FOnPlayerStateChanged OnPlayerStateReceived;

    void SetPawn(APawn* InPawn) override;

    // Called when player controller is ready to begin playing, good moment to initialize things like UI for which BeginPlay might be too early to set up
    // Especially in multiplayer clients where not all the data such as the PlayerState have been replicated properly for each client yet
    virtual void BeginPlayingState() override;

    // In order to keep our Blueprint implementation, we have to assign an event that can be placed in the Blueprint Editor that will be called from our CPP function
    // through the replication of the event on OnRep_PlayerState() once the server updates the client's SPlayerState
    UFUNCTION(BlueprintImplementableEvent)
    void BlueprintBeginPlayingState();

    void OnRep_PlayerState() override;
};
