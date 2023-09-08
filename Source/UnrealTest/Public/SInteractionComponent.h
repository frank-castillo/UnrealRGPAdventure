// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"

class USWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALTEST_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    UPROPERTY()
    AActor* FocusedActor;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<USWorldUserWidget> DefaultWidgetClass;

    UPROPERTY()
    USWorldUserWidget* DefaultWidgetInstance;

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float TraceDistance;

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float TraceRadius;

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    TEnumAsByte<ECollisionChannel> CollisionChannel;

    // Network Standard
    // For RPCs (Remote Procedure Calls) we follow the next naming convention
    // Server + Function name => Server RPC
    // Client + Function name => Client RPC
    // Multicast + Function name => Multicast RPC

    // We mark the function as a UFUNCTION and add the identifier SERVER
    // We also add the following:
    // Reliable = Will always arrive, eventually. Request will be re-sent unless an acknowledgment was received, like TCP/IP
    // Unreliable - Not guaranteed, packet can get lost and won't retry, like UDP/IP
    UFUNCTION(Server, Reliable)
    void ServerInteract(AActor* InFocus);

public:	

    void FindBestInteractable();

    void PrimaryIntetract();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
