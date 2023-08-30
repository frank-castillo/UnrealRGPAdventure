// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include <AIModule/Classes/Perception/PawnSensingComponent.h>
#include <AIModule/Classes/AIController.h>
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include <DrawDebugHelpers.h>
#include "SAttributeComponent.h"
#include <AIModule/Classes/BrainComponent.h>

// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

    AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributesComp"));

    TimeToHitParamName = "TimeToHit";

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
    AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
    AAIController* AIC = Cast<AAIController>(GetController());

    if (ensure(AIC))
    {
        AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
    }
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
    SetTargetActor(Pawn);
    DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.0f)
    {
        if (InstigatorActor != this)
        {
            SetTargetActor(InstigatorActor);
            DrawDebugString(GetWorld(), GetActorLocation(), "I WAS ATTACKED", nullptr, FColor::White, 4.0f, true);
        }

        GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

        if (NewHealth <= 0.0f)
        {
            // Stop BT
            AAIController* AIC = CastChecked<AAIController>(GetController());
            if (AIC)
            {
                AIC->GetBrainComponent()->StopLogic("Killed");
            }

            // Ragdoll
            GetMesh()->SetAllBodiesSimulatePhysics(true);
            GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

            // Set lifespan -> How long before we call destroy
            SetLifeSpan(10.0f);
        }
    }
}
