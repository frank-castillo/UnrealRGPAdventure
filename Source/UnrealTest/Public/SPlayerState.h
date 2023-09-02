// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

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
    float Credits;

public:

    UFUNCTION(BlueprintCallable, Category = "Credits")
    bool GrantCredits(const float NewCredits);
};
