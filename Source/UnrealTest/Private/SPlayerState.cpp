// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

ASPlayerState::ASPlayerState()
{
    Credits = 0;
}

bool ASPlayerState::GrantCredits(const float NewCredits)
{
    const float CurrentCredits = Credits;
    Credits += NewCredits;

    if (Credits > CurrentCredits)
    {
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("Error granting credits. Please check this issue."));

    return false;
}

bool ASPlayerState::DeductCredits(const float DeductedCredits)
{
    if (Credits == 0.0f)
    {
        return true;
    }

    const float CurrentCredits = Credits;
    Credits = FMath::Clamp(Credits -= DeductedCredits, 0.0f, CurrentCredits);

    if (Credits < CurrentCredits)
    {
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("Error deducting credits. Please check this issue."));

    return false;
}

float ASPlayerState::GetCredits() const
{
    return Credits;
}
