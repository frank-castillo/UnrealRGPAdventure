// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include <AIModule/Classes/EnvironmentQuery/EnvQueryManager.h>
#include <AIModule/Classes/EnvironmentQuery/EnvQueryTypes.h>
#include <AIModule/Classes/EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h>
#include "AI/SAICharacter.h"
#include "SAttributeComponent.h"
#include <EngineUtils.h>
#include <DrawDebugHelpers.h>
#include "SCharacter.h"
#include <Engine/EngineTypes.h>
#include "SPlayerState.h"
#include "SSaveGame.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/GameModeBase.h>
#include <GameFramework/GameStateBase.h>
#include "SGameplayInterface.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
    SpawnTimerInterval = 2.0f;
    RespawnDelay = 2.0f;
    AICreditValue = 10;
    DesiredPowerupCount = 10;
    RequiredPowerupDistance = 2000;

    SlotName = "SavedGamed01";

    PlayerStateClass = ASPlayerState::StaticClass();
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    LoadSaveGame();
}

void ASGameModeBase::StartPlay()
{
    Super::StartPlay();

    // continuous timer to spawn in more bots
    // Actual amount of bots and whether it is allowed to spawn determined by spawn logic
    GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

    // Spawn coin pickups
    if (ensure(PowerupClasses.Num() > 0))
    {
        // Run EQS to find potential power-up spawn locations
        UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);

        if (ensure(QueryInstance))
        {
            QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
        }
    }
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
    // Set timer with parameter
    ASCharacter* Player = Cast<ASCharacter>(VictimActor);

    if (Player)
    {
        // We do not store the character because if we reuse the handle, then we will never respawn the player that died as it would get overwritten
        // Using a local variable solves this as it will get instantiated in memory and live independent per character we have
        FTimerHandle TimerHandle_RespawnDelay;

        // Pass who we respawn
        FTimerDelegate Delegate;
        Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

        GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);

        UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
    }

    // Looman Implementation
    APawn* KillerPawn = Cast<APawn>(Killer);
    if (KillerPawn)
    {
        ASPlayerState* PS = KillerPawn->GetPlayerState<ASPlayerState>();

        if (PS)
        {
            PS->AddCredits(AICreditValue);
        }
    }

    // Own implementation
    /*else if (VictimActor->IsA<ASAICharacter>() && Killer->IsA<ASCharacter>())
    {
        Player = Cast<ASCharacter>(Killer);
        ASPlayerState* PlayerState = Cast<ASPlayerState>(Player->GetController()->PlayerState.Get());
        PlayerState->GrantCredits(AICreditValue);
    }*/
}

void ASGameModeBase::KillAll()
{
    for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
    {
        ASAICharacter* Bot = *It;
        USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);

        if (ensure(AttributeComp) && AttributeComp->IsAlive())
        {
            AttributeComp->Kill(this); // @fixme: Pass in player? for kill credit
        }
    }
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
    if (!CVarSpawnBots.GetValueOnGameThread())
    {
        UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
        return;
    }

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
        QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
    }
}

void ASGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
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

void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
    if (QueryStatus != EEnvQueryStatus::Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spawn powerup Query Failed!"));
        return;
    }

    // Get possible locations to spawn the item - same as bots
    TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

    // Keep used locations to easily check distance between points
    TArray<FVector> UsedLocations;

    int32 SpawnCounter = 0;

    // Break out if we reached the desired count or if we have no more potential positions remaining
    while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
    {
        // Pick a random location from remaining points.
        int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

        FVector PickedLocation = Locations[RandomLocationIndex];
        // Remove to avoid picking again
        Locations.RemoveAt(RandomLocationIndex);

        // Check minimum distance requirement
        bool bValidLocation = true;
        for (FVector OtherLocation : UsedLocations)
        {
            float DistanceTo = (PickedLocation - OtherLocation).Size();

            if (DistanceTo < RequiredPowerupDistance)
            {
                // Show skipped locations due to distance
                DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

                // too close, skip to next attempt
                bValidLocation = false;
                break;
            }
        }

        // Failed the distance test
        if (!bValidLocation)
        {
            continue;
        }

        // Pick a random powerup-class
        int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
        TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

        GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

        // Keep for distance checks
        UsedLocations.Add(PickedLocation);
        SpawnCounter++;
    }
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
    if (ensure(Controller))
    {
        // Remove control and detach from us -> this ensures a proper clean copy of the character
        Controller->UnPossess();
        RestartPlayer(Controller);
    }
}

void ASGameModeBase::WriteSaveGame()
{
    // Iterate all player states, we don't have ID to match yet (requires Stream or EOS)
    for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
    {
        ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
        if (PS)
        {
            PS->SavePlayerState(CurrentSaveGame);
            break; // Single player only at this point
        }
    }

    // Make a fresh list so we are not appending anything from a previous loaded file
    CurrentSaveGame->SavedActors.Empty();

    for (FActorIterator It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;

        // Only interested in our gameplay actors
        if (!Actor->Implements<USGameplayInterface>())
        {
            continue;
        }

        FActorSaveData ActorData;
        ActorData.ActorName = Actor->GetFName();
        ActorData.Transform = Actor->GetActorTransform();

        // Pass the array to fill with data from Actor
        FMemoryWriter MemWriter(ActorData.ByteData);

        FObjectAndNameAsStringProxyArchive Archive(MemWriter, true);

        // Find only variables with UPROPERTY(SaveGame)
        Archive.ArIsSaveGame = true;

        // Converts Actor's SaveGame UPROPERTIES into binary array
        Actor->Serialize(Archive);

        CurrentSaveGame->SavedActors.Add(ActorData);
    }

    UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0); // COnverted to binary and saved to disk
}

void ASGameModeBase::LoadSaveGame()
{
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

        if (CurrentSaveGame == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
            return;
        }

        UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

        for (FActorIterator It(GetWorld()); It; ++It)
        {
            AActor* Actor = *It;

            // Only interested in our gameplay actors
            if (!Actor->Implements<USGameplayInterface>())
            {
                continue;
            }

            for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
            {
                if (ActorData.ActorName == Actor->GetFName())
                {
                    Actor->SetActorTransform(ActorData.Transform);

                    // Pass the array to fill with data from Actor
                    FMemoryReader MemReader(ActorData.ByteData);

                    FObjectAndNameAsStringProxyArchive Archive(MemReader, true);

                    // Find only variables with UPROPERTY(SaveGame)
                    Archive.ArIsSaveGame = true;

                    // Converts binary array back into Actor's variables
                    Actor->Serialize(Archive);

                    ISGameplayInterface::Execute_OnActorLoaded(Actor);
                    
                    break;
                }
            }
        }
    }
    else
    {
        CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

        UE_LOG(LogTemp, Log, TEXT("Created New SaveGame Data."));
    }
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    // We call our logic before the Super so we can set our variables before BeginPlayingState is called in Player Controller
    // This ensure proper UI instantiation
    ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
    if (PS)
    {
        PS->LoadPlayerState(CurrentSaveGame);
    }

    Super::HandleStartingNewPlayer_Implementation(NewPlayer); // This function calls our BeginPlayingState in our Custom controller

    // We load our data as early as possible to avoid delaying the game or variables and widgets not being properly initialized
}
