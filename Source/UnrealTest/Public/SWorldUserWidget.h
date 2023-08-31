// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;

/**
 * 
 */
UCLASS()
class UNREALTEST_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    AActor* AttachedActor;

protected:

    UPROPERTY(EditAnywhere, Category = "UI")
    FVector WorldOffset;

    UPROPERTY(meta = (BindWidget))
    USizeBox* ParentSizeBox;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
