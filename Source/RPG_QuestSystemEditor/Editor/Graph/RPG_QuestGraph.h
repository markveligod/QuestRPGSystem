/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "RPG_QuestGraph.generated.h"

/**
 * 
 */
UCLASS()
class RPG_QUESTSYSTEMEDITOR_API URPG_QuestGraph : public UEdGraph
{
    GENERATED_BODY()

public:

    URPG_QuestGraph(const FObjectInitializer& ObjectInitializer);
    
    /** Returns the Quest Object that contains this graph */
    class URPG_QuestObjectBase* GetQuestObject() const;

    virtual void PreSave(FObjectPreSaveContext SaveContext) override;

};
