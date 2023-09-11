// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USSaveGame;

//Looman
// USeful to bind in gameplay
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

/**
 * 
 */
UCLASS()
class UNREALTEST_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:

    ASPlayerState();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing="OnRep_Credits", Category = "Credits")
    int32 Credits;

    // We keep the old value as to calculate the delta
    UFUNCTION()
    void OnRep_Credits(int32 OldCredits);

    // Multicast is more expensive as it would occupy more bandwidth. Using OnRep is easier as the variable itself is already being replicated,
    // so we get a less expensive automatic trigger.

public:

    UFUNCTION(BlueprintCallable, Category = "Credits")
    bool GrantCredits(const int32 NewCredits);

    UFUNCTION(BlueprintCallable, Category = "Credits")
    bool DeductCredits(const int32 NewCredits);

    UFUNCTION(BlueprintCallable, Category = "Credits")
    int32 GetCredits() const;

    // Looman Implementation
    UFUNCTION(BlueprintCallable, Category = "Credits")
    void AddCredits(int32 Delta);

    UFUNCTION(BlueprintCallable, Category = "Credits")
    bool RemoveCredits(int32 Delta);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCreditsChanged OnCreditsChanged;

    UFUNCTION(BlueprintNativeEvent)
    void SavePlayerState(USSaveGame* SaveObject);

    UFUNCTION(BlueprintNativeEvent)
    void LoadPlayerState(USSaveGame* SaveObject);
};
