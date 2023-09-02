// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ASCharacter::ASCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->SetupAttachment(RootComponent);

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp);

    InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));

    AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributesComp"));

    // This rotates the model to the input
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxJumpApexAttemptsPerSimulation = 1;

    bUseControllerRotationYaw = false;

    AttackAnimDelay = 0.2f;
    TimeToHitParamName = "TimeToHit";
    HandSocketName = "Muzzle_01";
}

void ASCharacter::HealSelf(float Amount)
{
    AttributeComp->ApplyHealthChange(this, Amount);
}

void ASCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

FVector ASCharacter::GetPawnViewLocation() const
{
    return CameraComp->GetComponentLocation();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // -- Rotation Visualization -- //
    const float DrawScale = 100.0f;
    const float Thickness = 5.0f;

    FVector LineStart = GetActorLocation();
    // Offset to the right of pawn
    LineStart += GetActorRightVector() * 100.0f;
    // Set line end in direction of the actor's forward
    FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
    // Draw Actor's Direction
    DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

    FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
    // Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
    DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
    PlayerInputComponent->BindAction("BlackHoleAbility", IE_Pressed, this, &ASCharacter::BlackHoleAbility);
    PlayerInputComponent->BindAction("DashAbility", IE_Pressed, this, &ASCharacter::DashAbility);
    PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
}

void ASCharacter::MoveForward(float Value)
{
    FRotator ControlRot = GetControlRotation();
    ControlRot.Pitch = 0.0f;
    ControlRot.Roll = 0.0f;

    // Function in character -> goes to character movement component
    AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
    FRotator ControlRot = GetControlRotation();
    ControlRot.Pitch = 0.0f;
    ControlRot.Roll = 0.0f;

    // x = Forward
    // Y = Right
    // Z = Up

    FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

    // Function in character -> goes to character movement component
    AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
    StartAttackEffects();

    GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, AttackAnimDelay);

    //GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
    // Ensure only triggers once after compilation
    // ensureAlways triggers every time it happens
    // check stops the engine and dead locks it
    // ensure does not ship with the build
    //if (ensure(ProjectileClass))
    //{
    //    FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

    //    // Transform Matrix
    //    FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);

    //    FActorSpawnParameters SpawnParams;
    //    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    //    SpawnParams.Instigator = this;

    //    GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
    //}
    SpawnProjectile(ProjectileClass);

}

void ASCharacter::DashAbility()
{
    StartAttackEffects();

    GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::DashAbility_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::DashAbility_TimeElapsed()
{
    SpawnProjectile(ProjectileDashClass);
}

void ASCharacter::BlackHoleAbility()
{
    StartAttackEffects();

    GetWorldTimerManager().SetTimer(TimerHandle_BlackHoleAttack, this, &ASCharacter::BlackHoleAbility_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::BlackHoleAbility_TimeElapsed()
{
    SpawnProjectile(ProjectileBlackHoleClass);
}

void ASCharacter::PrimaryInteract()
{
    if (InteractionComp)
    {
        InteractionComp->PrimaryIntetract();
    }
}

void ASCharacter::StartAttackEffects()
{
    PlayAnimMontage(AttackAnim);

    UGameplayStatics::SpawnEmitterAttached(CastingEffect, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
    // Always check to make sure we are passing the right class and the argument is not null
    if (ensureAlways(ClassToSpawn))
    {
        // Hash string method to find the socket name
        FVector HandLocation = GetMesh()->GetSocketLocation(HandSocketName);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Instigator = this;

        // Debug
        FCollisionShape Shape;
        Shape.SetSphere(20.0f);

        // Ignore Player
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(this);

        // Set up collision layers
        FCollisionObjectQueryParams CollisionObjectParams;
        CollisionObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
        CollisionObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
        CollisionObjectParams.AddObjectTypesToQuery(ECC_Pawn);

        // Begin the ray trace to line up projectile with crosshair, regardless if avatar is left or right side oriented
        // First get camera position
        FVector TraceStart = CameraComp->GetComponentLocation();

        // Then set the end point of the trace collision check. This is pointing towards the look at of the character.
        FVector TraceEnd = CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 5000);

        // Collision Logic - Check returns true if it collided with something
        FHitResult Hit;
        // Find new direction/rotation from Hand pointing direction to the impact point in the world
        FRotator ProjectionRotation;
        if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, CollisionObjectParams, Shape, CollisionParams))
        {
            // Adjust location to end up at crosshair look-at
            ProjectionRotation = FRotationMatrix::MakeFromX(Hit.ImpactPoint - HandLocation).Rotator();
        }
        else
        {
            ProjectionRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();
        }

        // Final Transform Matrix based on new X-aligned rotation and the hand position as origin
        FTransform SpawnTM = FTransform(ProjectionRotation, HandLocation);
        GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParams);
    }
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.0f)
    {
        GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
    }

    if (NewHealth <= 0.0f && Delta < 0.0f)
    {
        APlayerController* PC = Cast<APlayerController>(GetController());
        DisableInput(PC);
    }
}
