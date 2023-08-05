// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"

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
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
    Super::BeginPlay();

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

    FVector RightVEctor = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

    // Function in character -> goes to character movement component
    AddMovementInput(RightVEctor, Value);
}

void ASCharacter::PrimaryAttack()
{
    PlayAnimMontage(AttackAnim);

    GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);

    //GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
    // Ensure only triggers once after compilation
    // ensureAlways triggers every time it happens
    // check stops the engine and dead locks it
    // ensure does not ship with the build
    if (ensure(ProjectileClass))
    {
        FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

        // Transform Matrix
        FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Instigator = this;

        GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
    }
}

void ASCharacter::PrimaryInteract()
{
    if (InteractionComp)
    {
        InteractionComp->PrimaryIntetract();
    }
}
