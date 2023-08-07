// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class USAttributeComponent;
class UAnimMontage;

UCLASS()
class UNREALTEST_API ASCharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, Category = "Attack")
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Attack")
    TSubclassOf<AActor> ProjectileDashClass;

    UPROPERTY(EditAnywhere, Category = "Attack")
    TSubclassOf<AActor> ProjectileBlackHoleClass;

    UPROPERTY(EditAnywhere, Category = "Attack")
    UAnimMontage* AttackAnim;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    float AttackAnimDelay = 0.0f;

    FTimerHandle TimerHandle_PrimaryAttack;
    FTimerHandle TimerHandle_BlackHoleAttack;
    FTimerHandle TimerHandle_Dash;

public:
    // Sets default values for this character's properties
    ASCharacter();

protected:
    UPROPERTY(VisibleAnywhere)
        USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere)
        UCameraComponent* CameraComp;

    UPROPERTY(VisibleAnywhere)
        USInteractionComponent* InteractionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USAttributeComponent* AttributeComp;

    void MoveForward(float Value);

    void MoveRight(float Value);

    void PrimaryAttack();

    void PrimaryAttack_TimeElapsed();

    void DashAbility();

    void DashAbility_TimeElapsed();

    void BlackHoleAbility();

    void BlackHoleAbility_TimeElapsed();

    void PrimaryInteract();

    void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);

    UFUNCTION()
    void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

    virtual void PostInitializeComponents() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
