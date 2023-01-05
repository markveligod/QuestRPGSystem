#pragma once

#include "EdGraph/EdGraph.h"
#include "QuestGraph.generated.h"

UCLASS(MinimalAPI)
class UQuestGraph : public UEdGraph
{
    GENERATED_UCLASS_BODY()

public:

    /** Returns the Quest Object that contains this graph */
    class UQuestObject* GetQuestObject() const;
};