/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestGraphSchema.h"

#include "EdGraphNode_Comment.h"
#include "RPG_QuestGraph.h"
#include "AnimGraph/Private/AnimGraphNode_Sync.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphNodes/RPG_QuestGraphNode_Base.h"
#include "RPG_QuestSystem/RPG_Config/RPG_QuestSystemSettings.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectBase.h"

#define LOCTEXT_NAMESPACE "QuestSchema"

#define DISALLOW_CONNECT FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not implemented by this schema"))
#define ALLOW_CONNECT FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Allow connect pins"));

bool URPG_QuestGraphSchema::CanGraphBeDropped(TSharedPtr<FEdGraphSchemaAction> InAction) const
{
    if (FEdGraphSchemaAction_Task* Action = static_cast<FEdGraphSchemaAction_Task*>(InAction.Get()))
    {
        return true;
    }
    return false;
}

void URPG_QuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
    Super::GetGraphContextActions(ContextMenuBuilder);
}

void URPG_QuestGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    if (Context && Context->Node && Context->Node->IsA(URPG_QuestGraphNode_Base::StaticClass()))
    {
        FToolMenuSection& Section = Menu->AddSection(FName("QuestGraphSchemaNodeActions"), LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
        Section.AddMenuEntry(FGenericCommands::Get().Delete);
    }
    Super::GetContextMenuActions(Menu, Context);
}

void URPG_QuestGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
    Super::CreateDefaultNodesForGraph(Graph);

    URPG_QuestObjectBase* QuestObject = CastChecked<URPG_QuestObjectBase>(Graph.GetOuter());
    if (!QuestObject) return;

    const TArray<FRPG_TaskNodeData>& ArrayTaskNodes = QuestObject->GetArrayTaskNodes();
    for (const FRPG_TaskNodeData& NodeData : ArrayTaskNodes)
    {
        FGraphNodeCreator<URPG_QuestGraphNode_Base> NodeCreator(Graph);
        URPG_QuestGraphNode_Base* ResultRootNode = NodeCreator.CreateNode();
        if (!ResultRootNode) continue;

        ResultRootNode->TargetIndexTaskNode = NodeData.IndexNode;
        ResultRootNode->NodePosX = NodeData.NodePosition.X;
        ResultRootNode->NodePosY = NodeData.NodePosition.Y;
        NodeCreator.Finalize();
        SetNodeMetaData(ResultRootNode, FNodeMetadata::DefaultGraphNode);
        ResultRootNode->Modify();
        ResultRootNode->MarkPackageDirty();
    }

    AutoConnectNodeByDefault(Graph);
}

void URPG_QuestGraphSchema::AutoConnectNodeByDefault(UEdGraph& Graph) const
{
    for (auto Node : Graph.Nodes)
    {
        if (URPG_QuestGraphNode_Base* QuestNode = Cast<URPG_QuestGraphNode_Base>(Node.Get()))
        {
            FRPG_TaskNodeData* TaskNodeData = QuestNode->GetTaskNodeData();
            if (!TaskNodeData) continue;

            if (TaskNodeData->TypeNode == ERPG_TypeNode::StartNode)
            {
                if (TaskNodeData->StartConnectNode.OutConnectNode != INDEX_NONE)
                {
                    if (URPG_QuestGraphNode_Base* TargetNode = FindQuestGraphNodeByIndex(Graph, TaskNodeData->StartConnectNode.OutConnectNode))
                    {
                        UEdGraphPin* OutPin = QuestNode->FindPin(FName(PIN_TASK_OUT), EGPD_Output);
                        UEdGraphPin* InPin = TargetNode->FindPin(FName(PIN_TASK_IN), EGPD_Input);
                        if (OutPin && InPin)
                        {
                            OutPin->MakeLinkTo(InPin);
                        }
                    }
                }
            }

            if (TaskNodeData->TypeNode == ERPG_TypeNode::StandardNode)
            {
                if (TaskNodeData->StandardConnectNode.OutConnectNode != INDEX_NONE)
                {
                    if (URPG_QuestGraphNode_Base* TargetNode = FindQuestGraphNodeByIndex(Graph, TaskNodeData->StandardConnectNode.OutConnectNode))
                    {
                        UEdGraphPin* OutPin = QuestNode->FindPin(FName(PIN_TASK_OUT), EGPD_Output);
                        UEdGraphPin* InPin = TargetNode->FindPin(FName(PIN_TASK_IN), EGPD_Input);
                        if (OutPin && InPin)
                        {
                            OutPin->MakeLinkTo(InPin);
                        }
                    }
                }
            }
        }
    }
}

UEdGraphNode* URPG_QuestGraphSchema::CreateStandardNodeForGraph(UEdGraph* Graph, UClass* TaskNodeClass, const FVector2D& InLocationNode, const ERPG_TypeNode& TypeNode) const
{
    if (!Graph) return nullptr;
    URPG_QuestObjectBase* QuestObject = CastChecked<URPG_QuestGraph>(Graph)->GetQuestObject();
    if (!QuestObject) return nullptr;

    FGraphNodeCreator<URPG_QuestGraphNode_Base> NodeCreator(*Graph);
    URPG_QuestGraphNode_Base* ResultRootNode = NodeCreator.CreateNode();
    if (!ResultRootNode) return nullptr;

    FRPG_TaskNodeData* TaskNode = QuestObject->CreateNewTaskNode(TaskNodeClass, InLocationNode, TypeNode);
    ResultRootNode->TargetIndexTaskNode = TaskNode->IndexNode;
    ResultRootNode->NodePosX = InLocationNode.X;
    ResultRootNode->NodePosY = InLocationNode.Y;
    NodeCreator.Finalize();
    SetNodeMetaData(ResultRootNode, FNodeMetadata::DefaultGraphNode);
    ResultRootNode->Modify();
    ResultRootNode->MarkPackageDirty();
    return ResultRootNode;
}

const FPinConnectionResponse URPG_QuestGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
    if (!A || !B) return DISALLOW_CONNECT;
    if (A == B) return DISALLOW_CONNECT;
    if (A->Direction == EGPD_Input && B->Direction == EGPD_Input) return DISALLOW_CONNECT;
    if (A->Direction == EGPD_Output && B->Direction == EGPD_Output) return DISALLOW_CONNECT;
    if (A->GetOwningNode() == B->GetOwningNode()) return DISALLOW_CONNECT;

    URPG_QuestGraphNode_Base* A_GraphNode = Cast<URPG_QuestGraphNode_Base>(A->GetOwningNode());
    URPG_QuestGraphNode_Base* B_GraphNode = Cast<URPG_QuestGraphNode_Base>(B->GetOwningNode());
    if (!A_GraphNode || !B_GraphNode) return DISALLOW_CONNECT;

    if (A_GraphNode->GetIndexFromTaskNode(A->PinName) == INDEX_NONE && B_GraphNode->GetIndexFromTaskNode(B->PinName) == INDEX_NONE) return ALLOW_CONNECT;

    return DISALLOW_CONNECT;
}

bool URPG_QuestGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
    if (Super::TryCreateConnection(A, B))
    {
        URPG_QuestGraphNode_Base* A_GraphNode = Cast<URPG_QuestGraphNode_Base>(A->GetOwningNode());
        URPG_QuestGraphNode_Base* B_GraphNode = Cast<URPG_QuestGraphNode_Base>(B->GetOwningNode());
        if (A_GraphNode && B_GraphNode)
        {
            FRPG_TaskNodeData* A_TaskNodeData = A_GraphNode->GetTaskNodeData();
            FRPG_TaskNodeData* B_TaskNodeData = B_GraphNode->GetTaskNodeData();
            if (A_TaskNodeData && B_TaskNodeData)
            {
                if (A_TaskNodeData->TypeNode == ERPG_TypeNode::StartNode && B_TaskNodeData->TypeNode == ERPG_TypeNode::StandardNode)
                {
                    A_TaskNodeData->StartConnectNode.OutConnectNode = B_TaskNodeData->IndexNode;
                    B_TaskNodeData->StandardConnectNode.InConnectNode = A_TaskNodeData->IndexNode;
                }

                if (A_TaskNodeData->TypeNode == ERPG_TypeNode::StandardNode && B_TaskNodeData->TypeNode == ERPG_TypeNode::StartNode)
                {
                    B_TaskNodeData->StartConnectNode.OutConnectNode = A_TaskNodeData->IndexNode;
                    A_TaskNodeData->StandardConnectNode.InConnectNode = B_TaskNodeData->IndexNode;
                }

                if (A_TaskNodeData->TypeNode == ERPG_TypeNode::StandardNode && B_TaskNodeData->TypeNode == ERPG_TypeNode::StandardNode)
                {
                    if (A->PinName.ToString() == FString(PIN_TASK_IN))
                    {
                        A_TaskNodeData->StandardConnectNode.InConnectNode = B_TaskNodeData->IndexNode;
                    }

                    if (A->PinName.ToString() == FString(PIN_TASK_OUT))
                    {
                        A_TaskNodeData->StandardConnectNode.OutConnectNode = B_TaskNodeData->IndexNode;
                    }

                    if (B->PinName.ToString() == FString(PIN_TASK_IN))
                    {
                        B_TaskNodeData->StandardConnectNode.InConnectNode = A_TaskNodeData->IndexNode;
                    }

                    if (B->PinName.ToString() == FString(PIN_TASK_OUT))
                    {
                        B_TaskNodeData->StandardConnectNode.OutConnectNode = A_TaskNodeData->IndexNode;
                    }
                }
                return true;
            }
        }
    }

    return false;
}

FLinearColor URPG_QuestGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
    if (const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>())
    {
        return QuestSystemSettings->SchemePinColor.PinTypeColor;
    }
    return PinTypeColor;
}

FLinearColor URPG_QuestGraphSchema::GetSecondaryPinTypeColor(const FEdGraphPinType& PinType) const
{
    if (const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>())
    {
        return QuestSystemSettings->SchemePinColor.SecondaryPinTypeColor;
    }
    return SecondaryPinTypeColor;
}

URPG_QuestGraphNode_Base* URPG_QuestGraphSchema::FindQuestGraphNodeByIndex(UEdGraph& Graph, int32 TargetIndexNode) const
{
    for (auto Node : Graph.Nodes)
    {
        URPG_QuestGraphNode_Base* QuestNode = Cast<URPG_QuestGraphNode_Base>(Node.Get());
        if (!QuestNode) continue;
        if (QuestNode->TargetIndexTaskNode == TargetIndexNode)
        {
            return QuestNode;
        }
    }
    return nullptr;
}

#undef LOCTEXT_NAMESPACE
