// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>
#include "SActionComponent.generated.h"

class USAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALTEST_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USActionComponent();

    // We wanna make sure we can register actions and its public so any object that has or needs an action, can register itself
    UFUNCTION(BlueprintCallable, Category = "Actions")
    void AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    void RemoveAction(USAction* ActionToRemove);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    bool StartActionByName(AActor* Instigator, FName ActionName);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    bool StopActionByName(AActor* Instigator, FName ActionName);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    USAction* GetAction(TSubclassOf<USAction> ActionClass) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
    FGameplayTagContainer ActiveGameplayTags;

protected:

    UFUNCTION(Server, Reliable)
    void ServerStartAction(AActor* Instigator, FName ActionName);

    UFUNCTION(Server, Reliable)
    void ServerStopAction(AActor* Instigator, FName ActionName);

    UPROPERTY(EditAnywhere, Category = "Actions")
    TArray<TSubclassOf<USAction>> DefaultActions;

    // Marked as UPROPERTY so we tell Unreal there is memory it should keep track of
    // Marked as replicated so we can port this to the network
    UPROPERTY(Replicated)
    TArray<USAction*> Actions;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	

    bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
