// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

// Events allow for subscriptions and get notified just like in Unity
// The have to be declared just like they are in Unreal, making use of commas
// The first element on the delegate, is the parameter which is the event we will call = FOnHealthChanged
// The following arguments, delimited by commas, are the arguments we will pass additionally on the function so we can process the logic
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewRage, float, Delta);

// Better option: Shared event signature as both values share the same logic and arguments
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewValue, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALTEST_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    static USAttributeComponent* GetAttributes(AActor* FromActor);

    UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
    static bool IsActorAlive(AActor* Actor);

protected:

	// EditAnywhere - edit in BP editor and per-instance in level
	// VisibleAnywhere - "read-only" in editor and level. (Use for components)
	// EditDefaultsOnly - hide variable per-instance, edit in BP editor only
	// VisibleDefaultsOnly - 'read-only' access for variable, only in BP editor (uncommon)
	// EditInstanceOnly - allow only editing of instance (e.g. when placed in level)
	// --
	// BluePrintReadOnly - 'read-only' in the Blueprint scripting (does not affect 'details' panel)
	// BluePrintReadWrite - 'read-write' access in Blueprints
	// --
	// Category = "" - display only for detail panels and blueprint context menu

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float HealthMax;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
        float Rage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float RageMax;

	// HealthMax, Stamina, Strength

    // Because this is more a cosmetic change we don't need full response, however it is marked as Reliable because of our dead logic being inside this method
    // Specially because Health is already marked as replicated so we already get the updated value anytime it changes
    // @FIXME: Mark as unreliable once we move the state out of SCharacter into its own component
    UFUNCTION(NetMulticast, Reliable)
    void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastRageChanged(AActor* InstigatorActor, float NewRage, float Delta);

    // Possible Fix
    /*UPROPERTY(ReplicatedUsing="")
    bool bIsAlive;*/

public:

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    bool Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsFullHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealthMax() const;

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

    UFUNCTION(BlueprintCallable)
    float GetRage() const;

    UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRage(AActor* InstigatorActor, float Delta);

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnRageChanged;
};
