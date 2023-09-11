// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include <AIModule/Classes/Perception/PawnSensingComponent.h>
#include <AIModule/Classes/AIController.h>
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include <DrawDebugHelpers.h>
#include "SAttributeComponent.h"
#include <AIModule/Classes/BrainComponent.h>
#include "SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "SActionComponent.h"


// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributesComp"));
    ActionComp = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));

    TimeToHitParamName = "TimeToHit";
    TargetActorKey = "TargetActor";

    // Disabled on capsule to let projectiles pass through capsule and hit mesh instead
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

    // Enabled on mesh to react to incoming projectiles
    GetMesh()->SetGenerateOverlapEvents(true);

    // Ensures we receive a controlled when spawned in the level by our gamemode
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
        AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
    }
}

AActor* ASAICharacter::GetTargetActor() const
{
    AAIController* AIC = Cast<AAIController>(GetController());

    if (ensure(AIC))
    {
        return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
    }

    return nullptr;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
    if (GetTargetActor() != Pawn)
    {
        SetTargetActor(Pawn);

        MulticastPawnSeen();
    }
    //DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void ASAICharacter::MulticastPawnSeen_Implementation()
{
    USWorldUserWidget* NewWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);

    if (NewWidget)
    {
        NewWidget->AttachedActor = this;

        // Index of 10 (or anything higher than default of 0) places this on top of any other widget.
        // May end up behind the minion health bar otherwise.
        NewWidget->AddToViewport(10);
    }
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

        if (ActiveHealthBar == nullptr)
        {
            ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);

            if (ActiveHealthBar)
            {
                ActiveHealthBar->AttachedActor = this;
                ActiveHealthBar->AddToViewport();
            }
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

            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            GetCharacterMovement()->DisableMovement();

            // Set lifespan -> How long before we call destroy
            SetLifeSpan(10.0f);
        }
    }
}
