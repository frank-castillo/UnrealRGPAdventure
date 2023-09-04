// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SAction.generated.h"

class UWorld;

/**
 * Base USAction class from which other Actions will inherit its behaviour
 * Need to add Blueprintable so we can make child classes of this in the editor
 */
UCLASS(Blueprintable)
class UNREALTEST_API USAction : public UObject
{
	GENERATED_BODY()
	
public:

    // Marked native so the compiler understands that this function has a C++ implementation that can later on be overridden or expanded through Blueprints
    UFUNCTION(BlueprintNativeEvent, Category = "Action")
    void StartAction(AActor* Instigator);

    UFUNCTION(BlueprintNativeEvent, Category = "Action")
    void StopAction(AActor* Instigator);

    /* Action nickname to start/stop without a reference to the object*/
    UPROPERTY(EditDefaultsOnly, Category = "Action")
    FName ActionName;

    UWorld* GetWorld() const override;
};
