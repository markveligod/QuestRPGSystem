/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_QuestSystem/RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_QuestObjectDataTypes.generated.h"

UENUM()
enum class ERPG_TypeNode : uint8
{
    StartNode,
    StandardNode,
    FinishNode,
};

USTRUCT()
struct FRPG_TaskNodeData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    int32 IndexNode{INDEX_NONE};

    UPROPERTY()
    ERPG_TypeNode TypeNode{ERPG_TypeNode::StartNode};

    UPROPERTY()
    FVector2D NodePosition{FVector2D::ZeroVector};

    UPROPERTY(Instanced)
    class URPG_TaskNodeBase* TaskNodeBase{nullptr};

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
};
