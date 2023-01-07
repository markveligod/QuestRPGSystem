#pragma once

#include "QuestGraphNode_Base.h"
#include "QuestGraphNode.generated.h"

class UListTaskBase;
UCLASS(MinimalAPI)
class UQuestGraphNode : public UQuestGraphNode_Base
{
    GENERATED_UCLASS_BODY()

    /** The SoundNode this represents */
    UPROPERTY(VisibleAnywhere, instanced, Category=Quest)
    UListTaskBase* ListTask;

    /** Set the ListTask this represents (also assigns this to the SoundNode in Editor)*/
    void SetListTask(UListTaskBase* InListTask);
    /** Fix up the node's owner after being copied */
    void PostCopyNode();
    /** Create a new input pin for this node */
    void CreateInputPin();
    /** Add an input pin to this node and recompile the SoundCue */
    void AddInputPin();
    /** Remove a specific input pin from this node and recompile the SoundCue */
    void RemoveInputPin(UEdGraphPin* InGraphPin);
    /** Estimate the width of this Node from the length of its title */
    int32 EstimateNodeWidth() const;
    /** Checks whether an input can be added to this node */
    bool CanAddInputPin() const;

    // USoundCueGraphNode_Base interface
    virtual void CreateInputPins() override;
    // End of USoundCueGraphNode_Base interface

    // UEdGraphNode interface
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual void PrepareForCopying() override;
    virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
    virtual FText GetTooltipText() const override;
    virtual FString GetDocumentationExcerptName() const override;
    // End of UEdGraphNode interface

    // UObject interface
    virtual void PostLoad() override;
    virtual void PostEditImport() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
    // End of UObject interface

};
