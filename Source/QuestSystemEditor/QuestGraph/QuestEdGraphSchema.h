#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "QuestEdGraphSchema.generated.h"

class UQuestNode;
/** Action to add a node to the graph */
USTRUCT()
struct FQuestGraphSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	/** Class of node we want to create */
	UPROPERTY()
	class UClass* QuestNodeClass;


	FQuestGraphSchemaAction_NewNode() 
		: FEdGraphSchemaAction()
		, QuestNodeClass(nullptr)
	{}

	FQuestGraphSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FString& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, FText::FromString(InToolTip), InGrouping) 
		, QuestNodeClass(nullptr)
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface

private:
	/** Connects new node to output of selected nodes */
	void ConnectToSelectedNodes(UQuestNode* NewNodeclass, UEdGraph* ParentGraph) const;
};

/** Action to add nodes to the graph based on selected objects*/
USTRUCT()
struct FQuestGraphSchemaAction_NewFromSelected : public FQuestGraphSchemaAction_NewNode
{
	GENERATED_USTRUCT_BODY();

	FQuestGraphSchemaAction_NewFromSelected() 
		: FQuestGraphSchemaAction_NewNode()
	{}

	FQuestGraphSchemaAction_NewFromSelected(const FText& InNodeCategory, const FText& InMenuDesc, const FString& InToolTip, const int32 InGrouping)
		: FQuestGraphSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping) 
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface
};

/** Action to create new comment */
USTRUCT()
struct FQuestGraphSchemaAction_NewComment : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	FQuestGraphSchemaAction_NewComment() 
		: FEdGraphSchemaAction()
	{}

	FQuestGraphSchemaAction_NewComment(const FText& InNodeCategory, const FText& InMenuDesc, const FString& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, FText::FromString(InToolTip), InGrouping)
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface
};

/** Action to paste clipboard contents into the graph */
USTRUCT()
struct FQuestGraphSchemaAction_Paste : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	FQuestGraphSchemaAction_Paste() 
		: FEdGraphSchemaAction()
	{}

	FQuestGraphSchemaAction_Paste(const FText& InNodeCategory, const FText& InMenuDesc, const FString& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, FText::FromString(InToolTip), InGrouping)
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface
};

UCLASS(MinimalAPI)
class UQuestGraphSchema : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()

	/** Check whether connecting these pins would cause a loop */
	bool ConnectionCausesLoop(const UEdGraphPin* InputPin, const UEdGraphPin* OutputPin) const;

	/** Attempts to connect the output of multiple nodes to the inputs of a single one */
	void TryConnectNodes(const TArray<UQuestNode*>& OutputNodes, UQuestNode* InputNode) const;

	//~ Begin EdGraphSchema Interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
    virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
    virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
    virtual void CreateDefaultNodesForGraph(UEdGraph* Graph) const;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;
	virtual void DroppedAssetsOnGraph(const TArray<struct FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const override;
	virtual int32 GetNodeSelectionCount(const UEdGraph* Graph) const override;
	virtual TSharedPtr<FEdGraphSchemaAction> GetCreateCommentAction() const override;
	//~ End EdGraphSchema Interface

private:
	/** Adds actions for creating every type of SoundNode */
	void GetAllQuestNodeActions(FGraphActionMenuBuilder& ActionMenuBuilder, bool bShowSelectedActions) const;
	/** Adds action for creating a comment */
	void GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder, const UEdGraph* CurrentGraph = nullptr) const;

private:
	/** Generates a list of all available QuestNode classes */
	static void InitQuestNodeClasses();

	/** A list of all available QuestNode classes */
	static TArray<UClass*> QuestNodeClasses;
	/** Whether the list of QuestNode classes has been populated */
	static bool bQuestNodeClassesInitialized;
};
