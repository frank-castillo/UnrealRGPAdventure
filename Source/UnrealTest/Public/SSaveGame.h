// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSaveGame.generated.h"

USTRUCT()
struct FActorSaveData
{
    GENERATED_BODY()

public:

    // Identifier for which Actor this belongs to - ? Swap to FName?
    UPROPERTY()
    FName ActorName;

    // For movable actors, keep location, rotation, scale
    UPROPERTY()
    FTransform Transform;

    UPROPERTY()
    TArray<uint8> ByteData;
};

/**
 * 
 */
UCLASS()
class UNREALTEST_API USSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

    UPROPERTY()
    int32 Credits;

    UPROPERTY()
    TArray<FActorSaveData> SavedActors;
};
