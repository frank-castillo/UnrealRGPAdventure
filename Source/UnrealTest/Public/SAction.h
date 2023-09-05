// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <GameplayTagContainer.h>
#include "SAction.generated.h"

class UWorld;
class USActionComponent;

/**
 * Base USAction class from which other Actions will inherit its behaviour
 * Need to add Blueprintable so we can make child classes of this in the editor
 */
UCLASS(Blueprintable)
class UNREALTEST_API USAction : public UObject
{
	GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintNativeEvent, Category = "Action")
    bool CanStart(AActor* Instigator);

    // Marked native so the compiler understands that this function has a C++ implementation that can later on be overridden or expanded through Blueprints
    UFUNCTION(BlueprintNativeEvent, Category = "Action")
    void StartAction(AActor* Instigator);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
    void StopAction(AActor* Instigator);

    UFUNCTION(BlueprintCallable, Category = "Action")
    bool IsRunning() const;

    /* Action nickname to start/stop without a reference to the object*/
    UPROPERTY(EditDefaultsOnly, Category = "Action")
    FName ActionName;

    UWorld* GetWorld() const override;

protected:

    UFUNCTION(BlueprintCallable, Category = "Action")
    USActionComponent* GetOwningComponent() const;

    //Tags added to owning actor when activated, removed when action stops
    UPROPERTY(EditDefaultsOnly, Category = "Tags")
    FGameplayTagContainer GrantsTags;

    // Action can only start if OwningActor has none of these tags applied
    UPROPERTY(EditDefaultsOnly, Category = "Tags")
    FGameplayTagContainer BlockedTags;

    bool bIsRunning;
};
