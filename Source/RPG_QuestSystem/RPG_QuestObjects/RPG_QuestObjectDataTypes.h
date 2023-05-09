/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_QuestSystem/RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_QuestObjectDataTypes.generated.h"

/** @enum The type of node that determines the operation of the node **/
UENUM()
enum class ERPG_TypeNode : uint8
{
    StartNode,
    StandardNode,
    FinishNode,
};

/** @struct A data set describing the operation of the node **/
USTRUCT()
struct FRPG_TaskNodeData
{
    GENERATED_USTRUCT_BODY()

    /** @public Individual index when creating a node **/
    UPROPERTY()
    int32 IndexNode{INDEX_NONE};

    /** @public Type node **/
    UPROPERTY()
    ERPG_TypeNode TypeNode{ERPG_TypeNode::StartNode};

    /** @public Coordinates of the node position on the graph **/
    UPROPERTY()
    FVector2D NodePosition{FVector2D::ZeroVector};

    /** @public Task class for the standard node type **/
    UPROPERTY(Instanced)
    class URPG_TaskNodeBase* TaskNodeBase{nullptr};

    /** @public Outgoing node type to other nodes **/
    UPROPERTY()
    TArray<int32> OutNodes;

    void ClearData()
    {
        if (TaskNodeBase)
        {
            TaskNodeBase->MarkAsGarbage();
        }
        TaskNodeBase = nullptr;
    }

    bool operator==(const FRPG_TaskNodeData& Data) const { return Data.IndexNode == this->IndexNode; }

    friend FArchive& operator<<(FArchive& Ar, FRPG_TaskNodeData& InstanceData)
    {
        return Ar << InstanceData.IndexNode << InstanceData.TypeNode << InstanceData.NodePosition << InstanceData.TaskNodeBase << InstanceData.OutNodes;
    }

    FString ToString() const
    {
        if (TaskNodeBase)
        {
            return FString::Printf(TEXT("Index node: [%i] | TypeNode: [%s]\nDesc Task: [%s] | State task: [%s]"),
            IndexNode, *UEnum::GetValueAsString(TypeNode), *TaskNodeBase->GetDesc(), *UEnum::GetValueAsString(TaskNodeBase->GetStateTaskNode()));
        }
        return FString::Printf(TEXT("Index node: [%i] | TypeNode: [%s]\nDesc Task: [NONE] | State task: [NONE]"),
            IndexNode, *UEnum::GetValueAsString(TypeNode));
    }
};
