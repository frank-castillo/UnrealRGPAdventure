// Fill out your copyright notice in the Description page of Project Settings.

#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include <Net/UnrealNetwork.h>

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	HealthMax = 100.0f;
	Health = HealthMax;

    RageMax = 100.0f;
    Rage = 0.0f;

    SetIsReplicatedByDefault(true);
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
    if (FromActor)
    {
        return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
    }

    return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
    USAttributeComponent* AttributeComp = GetAttributes(Actor);

    if (AttributeComp)
    {
        return AttributeComp->IsAlive();
    }

    // We assume actor is dead if they don't have an attribute component
    return false;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
    return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

float USAttributeComponent::GetHealth() const
{
    return Health;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
    if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
    {
        return false;
    }

    if (Delta < 0.0f)
    {
        float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

        Delta *= DamageMultiplier;
    }

	float OldHealth = Health;
    float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax);
    float ActualDelta = NewHealth - OldHealth;

    // Is Server?
    if (GetOwner()->HasAuthority())
    {
        // We only change the health if we have authority
        Health = NewHealth;

        // This method ensures we call it on the server as well as on the clients
        // When ran on clients, it will call the method upon itself, and that is it. No other server calls.
        if (ActualDelta != 0.0f)
        {
            MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
        }

        // GameModes only exist on servers, never on clients, so there is no point on checking the gamemode logic on clients as it is useless

        // Died
        if (ActualDelta < 0.0f && Health == 0.0f)
        {
            // Auth stands for Authority -> This is used for Multiplayer games, where the only one that can access the game mode is the authority (server)
            ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();

            if (GM)
            {
                GM->OnActorKilled(GetOwner(), InstigatorActor);
            }
        }
    }
	
    // Single client method call
	//OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	return ActualDelta != 0;
}

float USAttributeComponent::GetRage() const
{
    return Rage;
}

bool USAttributeComponent::ApplyRage(AActor* InstigatorActor, float Delta)
{
    if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
    {
        return false;
    }

    if (Delta < 0.0f)
    {
        float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

        Delta *= DamageMultiplier;
    }

    float OldRage = Rage;
    Rage = FMath::Clamp(Rage + Delta, 0.0f, RageMax);
    float ActualDelta = Rage - OldRage;

    if (ActualDelta != 0.0f)
    {
        MulticastRageChanged(InstigatorActor, Rage, ActualDelta);
    }

    return ActualDelta != 0;
}

// Network code

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(USAttributeComponent, Health);
    DOREPLIFETIME(USAttributeComponent, HealthMax);

    // This is mostly used for optimization
    //DOREPLIFETIME_CONDITION(USAttributeComponent, HealthMax, COND_InitialOnly)
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
    OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
    OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}
