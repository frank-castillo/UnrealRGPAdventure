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
class UParticleSystem;
class USActionComponent;

UCLASS()
class UNREALTEST_API ASCharacter : public ACharacter
{
    GENERATED_BODY()

protected:

    /* VisibleAnywhere = read-only, still useful to view in-editor and enforce a convention. */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

    UPROPERTY(EditAnywhere, Category = "Attack")
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* CameraComp;

    UPROPERTY(VisibleAnywhere)
    USInteractionComponent* InteractionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USAttributeComponent* AttributeComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USActionComponent* ActionComp;

    void MoveForward(float Value);

    void MoveRight(float Value);

    void SprintStart();

    void SprintStop();

    void PrimaryAttack();

    void DashAbility();

    void BlackHoleAbility();

    void PrimaryInteract();

    UFUNCTION()
    void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

    virtual void PostInitializeComponents() override;

    virtual FVector GetPawnViewLocation() const override;

    //void PrimaryAttack_TimeElapsed();
    //void DashAbility_TimeElapsed();
    //void BlackHoleAbility_TimeElapsed();
    //void StartAttackEffects();
    //void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);

    //UPROPERTY(EditAnywhere, Category = "Attack")
    //TSubclassOf<AActor> ProjectileDashClass;

    //UPROPERTY(EditAnywhere, Category = "Attack")
    //TSubclassOf<AActor> ProjectileBlackHoleClass;

    //UPROPERTY(EditAnywhere, Category = "Attack")
    //UAnimMontage* AttackAnim;

    //UPROPERTY(EditDefaultsOnly, Category = "Attack")
    //float AttackAnimDelay = 0.0f;

    /* Particle System played during attack animation */
	//UPROPERTY(EditAnywhere, Category = "Attack")
	//UParticleSystem* CastingEffect;

    //FTimerHandle TimerHandle_PrimaryAttack;
    //FTimerHandle TimerHandle_BlackHoleAttack;
    //FTimerHandle TimerHandle_Dash;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Sets default values for this character's properties
    ASCharacter();

    UFUNCTION(Exec)
    void HealSelf(float Amount = 100.0f);
};
