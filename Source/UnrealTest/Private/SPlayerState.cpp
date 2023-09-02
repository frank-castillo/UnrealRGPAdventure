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
