// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASItemChest::ASItemChest()
{
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    RootComponent = BaseMesh;

    LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
    LidMesh->SetupAttachment(BaseMesh);

    TargetPitch = 110;

    // Assuring that it will REPLICATE and mark it up for Client Server sync
    bReplicates = true;
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
    bLidOpened = !bLidOpened;
    OnRep_LidOpened();
}

void ASItemChest::OnRep_LidOpened()
{
    float CurrPitch = bLidOpened ? TargetPitch : 0.0f;
    LidMesh->SetRelativeRotation(FRotator(CurrPitch, 0, 0));
}

void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // specify and when to update all clients in the network
    // C is class
    // V is value
    DOREPLIFETIME(ASItemChest, bLidOpened); // This will update all clients whenever the lid open bool changes
}
