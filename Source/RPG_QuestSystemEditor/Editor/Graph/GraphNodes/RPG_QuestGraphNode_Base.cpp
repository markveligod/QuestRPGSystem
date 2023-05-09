/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestGraphNode_Base.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectBase.h"

URPG_QuestGraphNode_Base::URPG_QuestGraphNode_Base() {}

#pragma region EdGraphNodeInterface

void URPG_QuestGraphNode_Base::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();

    FRPG_TaskNodeData* TaskNodeElem = GetTaskNodeData();
    if (!TaskNodeElem) return;
    if (TaskNodeElem->TypeNode == ERPG_TypeNode::StartNode)
    {
        OutputPin = CreatePin(EGPD_Output, TEXT("StartNode"), TEXT(""), PIN_TASK_OUT);
    }

    if (TaskNodeElem->TaskNodeBase && TaskNodeElem->TypeNode == ERPG_TypeNode::StandardNode)
    {
        FName PinCategory = *TaskNodeElem->TaskNodeBase->GetName();
        InPin = CreatePin(EGPD_Input, PinCategory, TEXT(""), PIN_TASK_IN);
        OutputPin = CreatePin(EGPD_Output, PinCategory, TEXT(""), PIN_TASK_OUT);
    }

    if (TaskNodeElem->TypeNode == ERPG_TypeNode::FinishNode)
    {
        InPin = CreatePin(EGPD_Input, TEXT("FinishNode"), TEXT(""), PIN_TASK_IN);
    }
}

void URPG_QuestGraphNode_Base::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    Super::GetNodeContextMenuActions(Menu, Context);
}

FText URPG_QuestGraphNode_Base::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    FRPG_TaskNodeData* TaskNodeElem = GetTaskNodeData();

    if (TaskNodeElem && TaskNodeElem->TypeNode == ERPG_TypeNode::StartNode)
    {
        return FText::FromString(FString::Printf(TEXT("#%i | START"), TaskNodeElem->IndexNode));
    }

    if (TaskNodeElem && TaskNodeElem->TypeNode == ERPG_TypeNode::StandardNode && TaskNodeElem->TaskNodeBase)
    {
        return FText::FromString(FString::Printf(TEXT("#%i | %s"), TaskNodeElem->IndexNode, *TaskNodeElem->TaskNodeBase->GetDesc()));
    }

    if (TaskNodeElem && TaskNodeElem->TypeNode == ERPG_TypeNode::FinishNode)
    {
        return FText::FromString(FString::Printf(TEXT("#%i | FINISH"), TaskNodeElem->IndexNode));
    }

    return Super::GetNodeTitle(TitleType);
}

FText URPG_QuestGraphNode_Base::GetTooltipText() const
{
    const FRPG_TaskNodeData* TaskNodeElem = GetTaskNodeData();

    if (TaskNodeElem && TaskNodeElem->TypeNode == ERPG_TypeNode::StartNode)
    {
        return FText::FromString(FString::Printf(TEXT("#%i | START | X=%i Y=%i"), TaskNodeElem->IndexNode, NodePosX, NodePosY));
    }

    if (TaskNodeElem && TaskNodeElem->TypeNode == ERPG_TypeNode::StandardNode && TaskNodeElem->TaskNodeBase)
    {
        return FText::FromString(FString::Printf(TEXT("#%i | %s | X=%i Y=%i\n"), TaskNodeElem->IndexNode, *TaskNodeElem->TaskNodeBase->GetDesc(), NodePosX, NodePosY));
    }

    if (TaskNodeElem && TaskNodeElem->TypeNode == ERPG_TypeNode::FinishNode)
    {
        return FText::FromString(FString::Printf(TEXT("#%i | FINISH | X=%i Y=%i"), TaskNodeElem->IndexNode, NodePosX, NodePosY));
    }

    return Super::GetTooltipText();
}

FLinearColor URPG_QuestGraphNode_Base::GetNodeTitleColor() const
{
    if (const URPG_QuestObjectBase* QuestObject = GetQuestObject())
    {
        return QuestObject->GetNodeTitleColor(TargetIndexTaskNode);
    }
    return Super::GetNodeTitleColor();
}

FLinearColor URPG_QuestGraphNode_Base::GetNodeCommentColor() const
{
    if (const URPG_QuestObjectBase* QuestObject = GetQuestObject())
    {
        return QuestObject->GetNodeCommentColor(TargetIndexTaskNode);
    }
    return Super::GetNodeCommentColor();
}

FLinearColor URPG_QuestGraphNode_Base::GetNodeBodyTintColor() const
{
    if (const URPG_QuestObjectBase* QuestObject = GetQuestObject())
    {
        return QuestObject->GetNodeBodyTintColor(TargetIndexTaskNode);
    }
    return Super::GetNodeBodyTintColor();
}

void URPG_QuestGraphNode_Base::NodeConnectionListChanged()
{
    Super::NodeConnectionListChanged();

    FRPG_TaskNodeData* TaskNodeElem = GetTaskNodeData();
    if (!TaskNodeElem) return;
    if (!OutputPin) return;

    TArray<int32> OutTargetNodes;
    for (UEdGraphPin* Pin : OutputPin->LinkedTo)
    {
        if (!Pin) continue;
        if (URPG_QuestGraphNode_Base* QuestGraphNode = Cast<URPG_QuestGraphNode_Base>(Pin->GetOwningNode()))
        {
            OutTargetNodes.Add(QuestGraphNode->TargetIndexTaskNode);
        }
    }
    TaskNodeElem->OutNodes = OutTargetNodes;
}

#pragma endregion

#pragma region ActionNode

URPG_QuestObjectBase* URPG_QuestGraphNode_Base::GetQuestObject() const
{
    return GetGraph() ? Cast<URPG_QuestObjectBase>(GetGraph()->GetOuter()) : nullptr;
}

FRPG_TaskNodeData* URPG_QuestGraphNode_Base::GetTaskNodeData() const
{
    URPG_QuestObjectBase* QuestObject = GetQuestObject();
    if (QuestObject)
    {
        return QuestObject->FindTaskNodeByElem(TargetIndexTaskNode);
    }
    return nullptr;
}

ERPG_TypeNode URPG_QuestGraphNode_Base::GetTypeTaskNode() const
{
    if (FRPG_TaskNodeData* TaskNodeElem = GetTaskNodeData())
    {
        return TaskNodeElem->TypeNode;
    }
    return ERPG_TypeNode::StartNode;
}

UEdGraphPin* URPG_QuestGraphNode_Base::GetGraphPinByName(FString NamePin)
{
    const auto FindElem = Pins.FindByPredicate([NamePin](UEdGraphPin* Pin) { return Pin ? Pin->PinName.ToString() == NamePin : false; });
    return FindElem ? *FindElem : nullptr;
}

#pragma endregion
