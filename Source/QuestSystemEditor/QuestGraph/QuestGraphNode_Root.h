#pragma once

#include "EdGraph/EdGraphNode.h"
#include "QuestGraphNode_Root.generated.h"

UCLASS()
class UQuestGraphNode_Root : public UEdGraphNode
{
    GENERATED_UCLASS_BODY()

#pragma region ObjectInterface

protected:

    // UObject interface
    virtual void PostLoad() override;
    virtual void PostEditImport() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
    // End of UObject interface

#pragma endregion
    
#pragma region EdGraphNodeInterface

public:

    // UEdGraphNode interface.
    virtual void AllocateDefaultPins() override;
    virtual void ReconstructNode() override;
    virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
    virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
    virtual FString GetDocumentationLink() const override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual void PrepareForCopying() override;
    virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
    virtual FText GetTooltipText() const override;
    virtual FString GetDocumentationExcerptName() const override;
    // End of UEdGraphNode interface.

#pragma endregion
};
