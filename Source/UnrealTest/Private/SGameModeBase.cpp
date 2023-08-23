// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include <AIModule/Classes/EnvironmentQuery/EnvQueryManager.h>
#include <AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h>
#include <AIModule/Classes/EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h>
#include "AI/SAICharacter.h"
#include "SAttributeComponent.h"
#include <EngineUtils.h>

ASGameModeBase::ASGameModeBase()
{
    SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
    Super::StartPlay();

    // continuous timer to spawn in more bots
    // Actual amount of bots and whether it is allowed to spawn determined by spawn logic
    GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
    // This is what gives us the event once it is done checking for the query
    // We return the best 5 percent random result from the query, as stated on the RunMode
    UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

    if (ensure(QueryInstance))
    {
        QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
    }
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
    if (QueryStatus != EEnvQueryStatus::Success)
    {
        // We use a log instead of an ensure as this query with the timer will be called every other 2 seconds and it can cause it to trigger every time it calls the query
        UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
        return;
    }

    int32 NrOfAliveBots = 0;
    for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
    {
        ASAICharacter* Bot = *It;

        USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributeComponent::StaticClass()));

        if (AttributeComp && AttributeComp->IsAlive())
        {
            NrOfAliveBots++;
        }
    }

    float MaxBotCount = 10.0f;

    // Allow for more spawns as time passes
    if (DifficultyCurve)
    {
        MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
    }

    if (NrOfAliveBots >= MaxBotCount)
    {
        return;
    }

    // Few frames after we trigger or EQS Query and understand where to spawn our bot
    TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

    if (Locations.IsValidIndex(0))
    {
        GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
    }
}
