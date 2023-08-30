// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include <AIModule/Classes/EnvironmentQuery/EnvQueryManager.h>
#include <AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h>
#include <AIModule/Classes/EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h>
#include "AI/SAICharacter.h"
#include "SAttributeComponent.h"
#include <EngineUtils.h>
#include <DrawDebugHelpers.h>

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
    int32 NrOfAliveBots = 0;
    for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
    {
        ASAICharacter* Bot = *It;

        // Before static function
        //USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributeComponent::StaticClass()));
        
        // After static function -> It can also be used as a separate node in blueprints now!
        USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);

        if (ensure(AttributeComp) && AttributeComp->IsAlive())
        {
            NrOfAliveBots++;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

    float MaxBotCount = 10.0f;

    // Allow for more spawns as time passes
    if (DifficultyCurve)
    {
        MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
    }

    if (NrOfAliveBots >= MaxBotCount)
    {
        UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn"))
            return;
    }

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

    // Few frames after we trigger or EQS Query and understand where to spawn our bot
    TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

    if (Locations.IsValidIndex(0))
    {
        GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

        DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
    }
}
