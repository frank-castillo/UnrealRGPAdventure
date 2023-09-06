// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

ASPlayerState::ASPlayerState()
{
    Credits = 0;
}

bool ASPlayerState::GrantCredits(const int32 NewCredits)
{
    const int32 CurrentCredits = Credits;
    Credits += NewCredits;

    if (Credits > CurrentCredits)
    {
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("Error granting credits. Please check this issue."));

    return false;
}

bool ASPlayerState::DeductCredits(const int32 DeductedCredits)
{
    if (Credits == 0.0f)
    {
        return true;
    }

    const int32 CurrentCredits = Credits;
    Credits = FMath::Clamp(Credits -= DeductedCredits, 0.0f, CurrentCredits);

    if (Credits < CurrentCredits)
    {
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("Error deducting credits. Please check this issue."));

    return false;
}

int32 ASPlayerState::GetCredits() const
{
    return Credits;
}

void ASPlayerState::AddCredits(int32 Delta)
{
    // Avoid user-error of adding a negative amount or zero
    if (!ensure(Delta > 0.0f))
    {
        return;
    }

    Credits += Delta;

    OnCreditsChanged.Broadcast(this, Credits, Delta);
}

bool ASPlayerState::RemoveCredits(int32 Delta)
{
    if (!ensure(Delta > 0.0f))
    {
        return false;
    }

    if (Credits < Delta)
    {
        // Not enough credits
        return false;
    }

    Credits -= Delta;
    OnCreditsChanged.Broadcast(this, Credits, Delta);
    return true;
}
