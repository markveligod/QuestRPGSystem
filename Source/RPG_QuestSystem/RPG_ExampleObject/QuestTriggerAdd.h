// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Engine/DataTable.h"
#include "QuestTriggerAdd.generated.h"

class UTextRenderComponent;
/**
 * 
 */
UCLASS(HideCategories = ("DataLayers", "AssetUserData", "RayTracing", "Mobile", "Actor", "Input", "WorldPartition", "Networking", "Replication", "Cooking", "Tags", "Rendering", "Physics", "HLOD", "Navigation", "TriggerBase"))
class RPG_QUESTSYSTEM_API AQuestTriggerAddBox : public ATriggerBox
{
	GENERATED_BODY()

public:

    AQuestTriggerAddBox();
    
protected:

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

    UPROPERTY(EditAnywhere)
    FDataTableRowHandle Quest;

    UPROPERTY(EditAnywhere)
    float WorldSizeText{50.0f};

    UPROPERTY(EditAnywhere)
    FColor TextColor{FColor::White};
    
    UPROPERTY()
    UTextRenderComponent* TextRender;
};
