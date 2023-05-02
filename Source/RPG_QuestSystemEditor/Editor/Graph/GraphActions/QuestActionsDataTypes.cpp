/** Copyright Mark Veligod. Published in 2023. **/

#include "QuestActionsDataTypes.h"
#include "RPG_QuestSystem/RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_QuestSystemEditor/Editor/Graph/RPG_QuestGraphSchema.h"

#define LOCTEXT_NAMESPACE "EdGraphSchemaAction"

FEdGraphSchemaAction_Task::FEdGraphSchemaAction_Task(FText InNodeCategory, URPG_TaskNodeBase* Node, const ERPG_TypeNode& TypeNode)
        : FEdGraphSchemaAction(MoveTemp(InNodeCategory),
        FText::FromString(Node->GetDesc()),
        FText::FromName(Node->GetClass()->GetFName()),0),
        NodeClass(Node),
        TypeNode(TypeNode)
{
    
}

FEdGraphSchemaAction_Task::FEdGraphSchemaAction_Task(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const ERPG_TypeNode& TypeNode)
        : FEdGraphSchemaAction(MoveTemp(InNodeCategory),
        MoveTemp(InMenuDesc),
        MoveTemp(InToolTip),0),
        TypeNode(TypeNode)
{
}

UEdGraphNode* FEdGraphSchemaAction_Task::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
    if (ParentGraph && TypeNode == ERPG_TypeNode::StandardNode)
    {
        return CastChecked<URPG_QuestGraphSchema>(ParentGraph->GetSchema())->CreateStandardNodeForGraph(ParentGraph, NodeClass->GetClass(), Location, TypeNode);
    }
    return nullptr;
}

#undef LOCTEXT_NAMESPACE
