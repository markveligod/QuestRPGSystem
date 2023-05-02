/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "GraphActions/QuestActionsDataTypes.h"
#include "RPG_QuestGraphSchema.generated.h"

enum class ERPG_TypeNode : uint8;
/**
 * 
 */
UCLASS()
class RPG_QUESTSYSTEMEDITOR_API URPG_QuestGraphSchema : public UEdGraphSchema
{
    GENERATED_BODY()

public:
    
    //~ Begin EdGraphSchema Interface
    virtual bool CanGraphBeDropped(TSharedPtr<FEdGraphSchemaAction> InAction) const override;
    virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
    virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
    virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
    virtual void AutoConnectNodeByDefault(UEdGraph& Graph) const;
    virtual UEdGraphNode* CreateStandardNodeForGraph(UEdGraph* Graph, UClass* TaskNodeClass, const FVector2D& InLocationNode, const ERPG_TypeNode& TypeNode) const;
    virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
    virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
    virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
    virtual FLinearColor GetSecondaryPinTypeColor(const FEdGraphPinType& PinType) const override;
    //~ End EdGraphSchema Interface

private:

    URPG_QuestGraphNode_Base* FindQuestGraphNodeByIndex(UEdGraph& Graph, int32 TargetIndexNode) const;

    FLinearColor PinTypeColor{FLinearColor::White};
    FLinearColor SecondaryPinTypeColor{FLinearColor::White};
};
