/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectDataTypes.h"
#include "QuestActionsDataTypes.generated.h"

class URPG_TaskNodeBase;

USTRUCT()
struct RPG_QUESTSYSTEMEDITOR_API FEdGraphSchemaAction_Task : public FEdGraphSchemaAction
{
    GENERATED_BODY()

    FEdGraphSchemaAction_Task() : FEdGraphSchemaAction() {}
    FEdGraphSchemaAction_Task(FText InNodeCategory, URPG_TaskNodeBase* Node, const ERPG_TypeNode& TypeNode);
    FEdGraphSchemaAction_Task(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const ERPG_TypeNode& TypeNode);

    virtual FName GetTypeId() const override { return StaticGetTypeId(); }

    virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override { Collector.AddReferencedObject(NodeClass); }

    static const FName& StaticGetTypeId()
    {
        static FName Type("FEdGraphSchemaAction_Task");
        return Type;
    }

    UPROPERTY()
    URPG_TaskNodeBase* NodeClass{nullptr};

    UPROPERTY()
    ERPG_TypeNode TypeNode{ERPG_TypeNode::StartNode};
};
