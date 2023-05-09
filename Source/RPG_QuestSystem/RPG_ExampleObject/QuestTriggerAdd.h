/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Engine/DataTable.h"
#include "QuestTriggerAdd.generated.h"

class UTextRenderComponent;
/**
 * @class Example of work adding a quest via trigger box
 */
UCLASS(HideCategories = ("DataLayers", "AssetUserData", "RayTracing", "Mobile", "Actor", "Input", "WorldPartition", "Networking", "Replication", "Cooking", "Tags", "Rendering", "Physics", "HLOD",
           "Navigation", "TriggerBase"))
class RPG_QUESTSYSTEM_API AQuestTriggerAddBox : public ATriggerBox
{
    GENERATED_BODY()

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
