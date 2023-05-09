/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectDataTypes.h"
#include "RPG_QuestGraphNode_Base.generated.h"

#define PIN_TASK_IN TEXT("In")
#define PIN_TASK_OUT TEXT("Out")

/**
 *
 */
UCLASS()
class RPG_QUESTSYSTEMEDITOR_API URPG_QuestGraphNode_Base : public UEdGraphNode
{
    GENERATED_BODY()

    URPG_QuestGraphNode_Base();

    virtual bool CanUserDeleteNode() const override { return true; }
    virtual bool CanDuplicateNode() const override { return false; }

#pragma region EdGraphNodeInterface

public:
    // UEdGraphNode interface.
    virtual void AllocateDefaultPins() override;
    virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    virtual FLinearColor GetNodeTitleColor() const override;
    virtual FLinearColor GetNodeCommentColor() const override;
    virtual FLinearColor GetNodeBodyTintColor() const override;
    virtual void NodeConnectionListChanged() override;
    // End of UEdGraphNode interface.

#pragma endregion

#pragma region DataNode

public:
    int32 TargetIndexTaskNode{INDEX_NONE};

    UEdGraphPin* OutputPin{nullptr};
    UEdGraphPin* InPin{nullptr};

#pragma endregion

#pragma region ActionNode

public:
    FRPG_TaskNodeData* GetTaskNodeData() const;
    ERPG_TypeNode GetTypeTaskNode() const;
    UEdGraphPin* GetGraphPinByName(FString NamePin);

private:
    URPG_QuestObjectBase* GetQuestObject() const;

#pragma endregion
};
