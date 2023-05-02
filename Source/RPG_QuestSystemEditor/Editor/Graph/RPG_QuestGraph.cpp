/** Copyright Mark Veligod. Published in 2023. **/


#include "RPG_QuestGraph.h"
#include "GraphNodes/RPG_QuestGraphNode_Base.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectBase.h"
#include "UObject/ObjectSaveContext.h"

URPG_QuestGraph::URPG_QuestGraph(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

URPG_QuestObjectBase* URPG_QuestGraph::GetQuestObject() const
{
    return CastChecked<URPG_QuestObjectBase>(GetOuter());
}

void URPG_QuestGraph::PreSave(FObjectPreSaveContext SaveContext)
{
    for (auto& Node : Nodes)
    {
        if (URPG_QuestGraphNode_Base* QuestGraphNode = Cast<URPG_QuestGraphNode_Base>(Node.Get()))
        {
            if (FRPG_TaskNodeData* TaskNodeElem = QuestGraphNode->GetTaskNodeData())
            {
                TaskNodeElem->NodePosition = FVector2D(QuestGraphNode->NodePosX, QuestGraphNode->NodePosY);
            }
        }
    }
    Super::PreSave(SaveContext);
}
