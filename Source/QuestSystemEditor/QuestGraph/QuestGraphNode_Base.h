﻿#pragma once

#include "EdGraph/EdGraphNode.h"
#include "QuestGraphNode_Base.generated.h"

UCLASS(MinimalAPI)
class UQuestGraphNode_Base : public UEdGraphNode
{
    GENERATED_UCLASS_BODY()

public:
    
    /** Create all of the input pins required */
    virtual void CreateInputPins() {};
    /** Is this the undeletable root node */
    virtual bool IsRootNode() const { return bRootNode; }
    /** Change state root node **/
    void ChangeStateRoot(const bool bNewState) { bRootNode = bNewState; }

    /** Get the Output Pin (should only ever be one) */
    class UEdGraphPin* GetOutputPin();
    /** Get all of the Input Pins */
    void GetInputPins(TArray<class UEdGraphPin*>& OutInputPins);
    /** Get a single Input Pin via its index */
    class UEdGraphPin* GetInputPin(int32 InputIndex);
    /** Get the current Input Pin count */
    int32 GetInputCount() const;

    /**
     * Handles inserting the node between the FromPin and what the FromPin was original connected to
     *
     * @param FromPin			The pin this node is being spawned from
     * @param NewLinkPin		The new pin the FromPin will connect to
     * @param OutNodeList		Any nodes that are modified will get added to this list for notification purposes
     */
    void InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList);

    // UEdGraphNode interface.
    virtual void AllocateDefaultPins() override;
    virtual void ReconstructNode() override;
    virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
    virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
    virtual FString GetDocumentationLink() const override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    // End of UEdGraphNode interface.

    /** @public Setup new name node **/
    void SetupNameNode(const FText NewNameNode) { NameNode = NewNameNode; }
    
private:

    bool bRootNode{false};
    FText NameNode{};
};