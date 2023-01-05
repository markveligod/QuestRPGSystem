
/***
 * UQuestGraph.cpp
 ***/

#include "QuestGraph.h"
#include "QuestRPGSystem/Objects/QuestObject.h"

UQuestGraph::UQuestGraph(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

UQuestObject* UQuestGraph::GetQuestObject() const
{
    return CastChecked<UQuestObject>(GetOuter());
}
