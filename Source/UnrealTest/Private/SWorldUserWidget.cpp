// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include <UMG/Public/Blueprint/WidgetLayoutLibrary.h>
#include <UMG/Public/Components/SizeBox.h>

void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    FVector2D ScreenPosition;

    if (!IsValid(AttachedActor))
    {
        RemoveFromParent();

        UE_LOG(LogTemp, Warning, TEXT("AttachedActor no longer valid, removing health widget"));

        return;
    }

    if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition))
    {
        float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

        ScreenPosition /= Scale;

        if (ParentSizeBox)
        {
            ParentSizeBox->SetRenderTranslation(ScreenPosition);
        }
    }
}
