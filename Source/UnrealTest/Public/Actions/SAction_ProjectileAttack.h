// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

class UAnimMontage;
class UParticleSystem;

/**
 * 
 */
UCLASS()
class UNREALTEST_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()
	
public:

    USAction_ProjectileAttack();

    virtual void StartAction_Implementation(AActor* Instigator) override;

protected:

    UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

    UPROPERTY(EditAnywhere, Category = "Attack")
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Attack")
    UAnimMontage* AttackAnim;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    float AttackAnimDelay = 0.0f;

    /* Particle System played during attack animation */
	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastingEffect;

    // Which character instigated the attack
    UFUNCTION()
    void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);
};
