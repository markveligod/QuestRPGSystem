/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestSystemSettings.h"
#include "RPG_QuestObjects/RPG_QuestObjectBase.h"
#include "RPG_QuestObjects/RPG_QuestObjectDataTypes.h"

URPG_QuestSystemSettings::URPG_QuestSystemSettings()
{
    NameCategoryByTypeNode.Add(ERPG_TypeNode::StartNode, TEXT("Tasks"));
    NameCategoryByTypeNode.Add(ERPG_TypeNode::StandardNode, TEXT("Tasks"));
    NameCategoryByTypeNode.Add(ERPG_TypeNode::FinishNode, TEXT("Tasks"));
    SupportQuestClass = URPG_QuestObjectBase::StaticClass();
}
