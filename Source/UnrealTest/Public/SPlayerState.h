// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Credits")
    int32 Credits;

public:

    UFUNCTION(BlueprintCallable, Category = "Credits")
    bool GrantCredits(const int32 NewCredits);

    UFUNCTION(BlueprintCallable, Category = "Credits")
    bool DeductCredits(const int32 NewCredits);

    UFUNCTION(BlueprintCallable, Category = "Credits")
    int32 GetCredits() const;

    // Looman Implementation
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCreditsChanged OnCreditsChanged;

    UFUNCTION(BlueprintCallable, Category = "Credits")
    void AddCredits(int32 Delta);

    UFUNCTION(BlueprintCallable, Category = "Credits")
    bool RemoveCredits(int32 Delta);
};
