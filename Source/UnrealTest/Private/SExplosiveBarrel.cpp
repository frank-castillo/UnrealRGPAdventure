// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include <PhysicsEngine/RadialForceComponent.h>

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");

    // Set Physics
    StaticMeshComp->SetSimulatePhysics(true);
    // Enabling Simulate physics automatically changes the Profile to PhysicsActor in Blueprint, in C++ we need to change this manually.
    //StaticMeshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
    
    StaticMeshComp->SetCollisionProfileName("ExplosiveBarrel");

    RootComponent = StaticMeshComp;

    RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
    RadialForceComp->SetupAttachment(StaticMeshComp);
    
    // Leaving this on applies small constant force via component 'tick' (Optional)
    RadialForceComp->SetAutoActivate(false);

    RadialForceComp->Radius = 500.0f;
    RadialForceComp->ImpulseStrength = 2000.0f;
    RadialForceComp->bImpulseVelChange = true;

    // Default constructor of component already adds 4 object types to affect, excluding WorldDynamic
    RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

void ASExplosiveBarrel::PostInitializeComponents()
{
    // Always call parent function
    Super::PostInitializeComponents();

    StaticMeshComp->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);
}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    RadialForceComp->FireImpulse();

    UE_LOG(LogTemp, Log, TEXT("OnActorHit in Explosive Barrel"));

    // %s = string
    // %f = float
    // logs: "OtherActor: MyActor_1, at gametime: 124.4" 
    UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s, at game time: %f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

    FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
    DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);

    // Detailed info on logging in ue4
    // https://nerivec.github.io/old-ue4-wiki/pages/logs-printing-messages-to-yourself-during-runtime.html

    OtherActor->Destroy();
    Destroy();
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

