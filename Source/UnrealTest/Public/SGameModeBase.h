// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h>
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class USSaveGame;

/**
 * 
 */
UCLASS()
class UNREALTEST_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

    FString SlotName;

    UPROPERTY()
    USSaveGame* CurrentSaveGame;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	UEnvQuery* PowerupSpawnQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Respawn")
    float RespawnDelay;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    int32 DesiredPowerupCount;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    float RequiredPowerupDistance;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    TArray<TSubclassOf<AActor>> PowerupClasses;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    int32 AICreditValue;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

    UFUNCTION()
    void OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

    UFUNCTION()
    void RespawnPlayerElapsed(AController* Controller);

public:

	ASGameModeBase();

    void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

    // Marked as virtual so we can override in child classes
    virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

    UFUNCTION(Exec)
    void KillAll();

    // Save and Load
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void WriteSaveGame();

    void LoadSaveGame();

    void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
