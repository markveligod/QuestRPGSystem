/*=============================================================================
	QuestGraphSchema.cpp
=============================================================================*/
#include "QuestEdGraphSchema.h"
#include "UnrealEd.h"
#include "ScopedTransaction.h"
#include "GraphEditor.h"
#include "GraphEditorActions.h"
#include "QuestGraph.h"
#include "QuestGraphNode_Base.h"
#include "QuestGraphNode_Root.h"
#include "Engine/Selection.h"
#include "Framework/Commands/GenericCommands.h"
#include "QuestRPGSystem/Objects/QuestObject.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "QuestSystemEditor/Utils/QuestEditorUtilities.h"

#define LOCTEXT_NAMESPACE "QuestSchema"

TArray<UClass*> UQuestGraphSchema::QuestNodeClasses;
bool UQuestGraphSchema::bQuestNodeClassesInitialized = false;

/////////////////////////////////////////////////////
// FSoundCueGraphSchemaAction_NewNode

UEdGraphNode* FQuestGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	// check(QuestNodeClass);
	//
	// UQuestObject* QuestObject = CastChecked<UQuestGraph>(ParentGraph)->GetSoundCue();
	// const FScopedTransaction Transaction( LOCTEXT("QuestEditorNewSoundNode", "Quest Editor: New Quest Node") );
	// ParentGraph->Modify();
	// QuestObject->Modify();
	//
	// UQuestNode* NewNode = QuestObject->ConstructSoundNode<UQuestNode>(QuestNodeClass, bSelectNewNode);
	//
	// // If this node allows >0 children but by default has zero - create a connector for starters
	// if (NewNode->GetMaxChildNodes() > 0 && NewNode->ChildNodes.Num() == 0)
	// {
	// 	NewNode->CreateStartingConnectors();
	// }
	//
	// // Attempt to connect inputs to selected nodes, unless we're already dragging from a single output
	// if (FromPin == nullptr || FromPin->Direction == EGPD_Input)
	// {
	// 	ConnectToSelectedNodes(NewNode, ParentGraph);
	// }
	//
	// NewNode->GraphNode->NodePosX = Location.X;
	// NewNode->GraphNode->NodePosY = Location.Y;
	//
	// NewNode->GraphNode->AutowireNewNode(FromPin);
	//
	// QuestObject->PostEditChange();
	// QuestObject->MarkPackageDirty();
	//
	// return NewNode->GraphNode;
    return nullptr;
}

void FQuestGraphSchemaAction_NewNode::ConnectToSelectedNodes(UListTaskBase* NewListTaskClass, class UEdGraph* ParentGraph) const
{
	// only connect if node can have many children
	// if (NewNode->GetMaxChildNodes() > 1)
	// {
	// 	const FGraphPanelSelectionSet SelectedNodes = FQuestEditorUtilities::GetSelectedNodes(ParentGraph);
	//
	// 	TArray<USoundNode*> SortedNodes;
	// 	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	// 	{
	// 		UQuestGraphNode* SelectedNode = Cast<UQuestGraphNode>(*NodeIt);
	//
	// 		if (SelectedNode)
	// 		{
	// 			// Sort the nodes by y position
	// 			bool bInserted = false;
	// 			for (int32 Index = 0; Index < SortedNodes.Num(); ++Index)
	// 			{
	// 				if (SortedNodes[Index]->GraphNode->NodePosY > SelectedNode->NodePosY)
	// 				{
	// 					SortedNodes.Insert(SelectedNode->SoundNode, Index);
	// 					bInserted = true;
	// 					break;
	// 				}
	// 			}
	// 			if (!bInserted)
	// 			{
	// 				SortedNodes.Add(SelectedNode->SoundNode);
	// 			}
	// 		}
	// 	}
	// 	if (SortedNodes.Num() > 1)
	// 	{
	// 		CastChecked<UQuestGraphSchema>(NewNode->GraphNode->GetSchema())->TryConnectNodes(SortedNodes, NewNode);
	// 	}
	// }
}

/////////////////////////////////////////////////////
// FSoundCueGraphSchemaAction_NewFromSelected

UEdGraphNode* FQuestGraphSchemaAction_NewFromSelected::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	UQuestObject* QuestObject = CastChecked<UQuestGraph>(ParentGraph)->GetQuestObject();
	const FScopedTransaction Transaction( LOCTEXT("QuestEditorNewFromSelection", "Quest Editor: New From Selection") );
	ParentGraph->Modify();
	QuestObject->Modify();

	UEdGraphNode* CreatedNode = nullptr;

	FVector2D NodeStartLocation = Location;

	if (QuestNodeClass)
	{
		// If we will create another node, move wave nodes out of the way.
		NodeStartLocation.X -= 200;
	}

	TArray<USoundWave*> SelectedWaves;
	TArray<USoundNode*> CreatedPlayers;
	GEditor->GetSelectedObjects()->GetSelectedObjects<USoundWave>(SelectedWaves);
	// FQuestEditorUtilities::CreateWaveContainers(SelectedWaves, QuestObject, CreatedPlayers, NodeStartLocation);
	//
	// if (QuestNodeClass)
	// {
	// 	UQuestNode* NewNode = QuestObject->ConstructSoundNode<UQuestNode>(QuestNodeClass, bSelectNewNode);
	// 	UEdGraphNode* NewGraphNode = NewNode->GraphNode;
	// 	const UQuestGraphSchema* NewSchema = CastChecked<UQuestGraphSchema>(NewGraphNode->GetSchema());
	//
	// 	// If this node allows >0 children but by default has zero - create a connector for starters
	// 	if (NewNode->GetMaxChildNodes() > 0 && NewNode->ChildNodes.Num() == 0)
	// 	{
	// 		NewNode->CreateStartingConnectors();
	// 	}
	//
	// 	NewSchema->TryConnectNodes(CreatedPlayers, NewNode);
	//
	// 	NewGraphNode->NodePosX = Location.X;
	// 	NewGraphNode->NodePosY = Location.Y;
	//
	// 	CreatedNode = NewNode->GraphNode;
	// }
	// else
	// {
	// 	if (CreatedPlayers.Num() > 0)
	// 	{
	// 		CreatedNode = CreatedPlayers[0]->GraphNode;
	// 	}
	// }

	if (CreatedNode)
	{
		CreatedNode->AutowireNewNode(FromPin);
	}

	QuestObject->PostEditChange();
	QuestObject->MarkPackageDirty();

	return CreatedNode;
}

/////////////////////////////////////////////////////
// FSoundCueGraphSchemaAction_NewComment

UEdGraphNode* FQuestGraphSchemaAction_NewComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	// Add menu item for creating comment boxes
	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2D SpawnLocation = Location;

	FSlateRect Bounds;
	// if (FQuestEditorUtilities::GetBoundsForSelectedNodes(ParentGraph, Bounds, 50.0f))
	// {
	// 	CommentTemplate->SetBounds(Bounds);
	// 	SpawnLocation.X = CommentTemplate->NodePosX;
	// 	SpawnLocation.Y = CommentTemplate->NodePosY;
	// }

	return FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation);
}

/////////////////////////////////////////////////////
// FQuestGraphSchemaAction_Paste

UEdGraphNode* FQuestGraphSchemaAction_Paste::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
    FQuestEditorUtilities::PasteNodesHere(ParentGraph, Location);
	return nullptr;
}

/////////////////////////////////////////////////////
// UQuestGraphSchema

UQuestGraphSchema::UQuestGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UQuestGraphSchema::ConnectionCausesLoop(const UEdGraphPin* InputPin, const UEdGraphPin* OutputPin) const
{
	UQuestGraphNode_Base* InputNode = Cast<UQuestGraphNode_Base>(InputPin->GetOwningNode());

	// if (InputNode)
	// {
	// 	// Only nodes representing SoundNodes have outputs
	// 	UQuestGraphNode* OutputNode = CastChecked<UQuestGraphNode>(OutputPin->GetOwningNode());
	//
	// 	// Grab all child nodes. We can't just test the output because 
	// 	// the loop could happen from any additional child nodes. 
	// 	TArray<USoundNode*> Nodes;
	// 	OutputNode->QuestNode->GetAllNodes(Nodes);
	//
	// 	// If our test input is in that set, return true.
	// 	return Nodes.Contains(InputNode->QuestNode);
	// }

	// Simple connection to root node
	return false;
}

void UQuestGraphSchema::TryConnectNodes(const TArray<UListTaskBase*>& OutputNodes, UListTaskBase* InputNode) const
{
	// for (int32 Index = 0; Index < OutputNodes.Num(); Index++)
	// {
	// 	if ( Index < InputNode->GetMaxChildNodes() )
	// 	{
	// 		if (Index >= InputNode->GetGraphNode()->GetInputCount())
	// 		{
	// 			InputNode->GetGraphNode()->CreateInputPin();
	// 		}
	// 		TryCreateConnection( InputNode->GetGraphNode()->GetInputPin(Index), OutputNodes[Index]->GetGraphNode()->GetOutputPin() );
	// 	}
	// }
}

void UQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	GetAllQuestNodeActions(ContextMenuBuilder, true);

	GetCommentAction(ContextMenuBuilder, ContextMenuBuilder.CurrentGraph);

	if (!ContextMenuBuilder.FromPin)
	{
		TSharedPtr<FQuestGraphSchemaAction_Paste> NewAction( new FQuestGraphSchemaAction_Paste(FText::GetEmpty(), LOCTEXT("Actions", "Create Node Task"), TEXT(""), 0) );
		ContextMenuBuilder.AddAction( NewAction );
	}
}

void UQuestGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    if (Context->Node && Context->Node->IsA(UQuestGraphNode_Base::StaticClass()))
    {
        FToolMenuSection& Section = Menu->AddSection(FName("QuestGraphSchemaNodeActions"), LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
        Section.AddMenuEntry(FGenericCommands::Get().Delete);
        Section.AddMenuEntry(FGenericCommands::Get().Cut);
        Section.AddMenuEntry(FGenericCommands::Get().Copy);
        Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
        Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
    }
    
    Super::GetContextMenuActions(Menu, Context);
}

void UQuestGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
}

void UQuestGraphSchema::CreateDefaultNodesForGraph(UEdGraph* Graph) const
{
    const int32 RootNodeHeightOffset = -58;

    // Create the result node
    FGraphNodeCreator<UQuestGraphNode_Root> NodeCreator(*Graph);
    UQuestGraphNode_Root* ResultRootNode = NodeCreator.CreateNode();
    ResultRootNode->NodePosY = RootNodeHeightOffset;
    NodeCreator.Finalize();
    SetNodeMetaData(ResultRootNode, FNodeMetadata::DefaultGraphNode);
    ResultRootNode->MarkPackageDirty();
}

void UQuestGraphSchema::CreateStandardNodeForGraph(UEdGraph* Graph, const FVector2D& InLocationNode) const
{
    // Create the result node
    FGraphNodeCreator<UQuestGraphNode_Base> NodeCreator(*Graph);
    UQuestGraphNode_Base* ResultRootNode = NodeCreator.CreateNode();
    ResultRootNode->NodePosX = InLocationNode.X;
    ResultRootNode->NodePosY = InLocationNode.Y;
    NodeCreator.Finalize();
    SetNodeMetaData(ResultRootNode, FNodeMetadata::DefaultGraphNode);
    ResultRootNode->MarkPackageDirty();
}

const FPinConnectionResponse UQuestGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionSameNode", "Both are on the same node"));
	}

	// Compare the directions
	const UEdGraphPin* InputPin = nullptr;
	const UEdGraphPin* OutputPin = nullptr;

	if (!CategorizePinsByDirection(PinA, PinB, /*out*/ InputPin, /*out*/ OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionIncompatible", "Directions are not compatible"));
	}

	if (ConnectionCausesLoop(InputPin, OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionLoop", "Connection would cause loop"));
	}

	// Break existing connections on inputs only - multiple output connections are acceptable
	if (InputPin->LinkedTo.Num() > 0)
	{
		ECanCreateConnectionResponse ReplyBreakOutputs;
		if (InputPin == PinA)
		{
			ReplyBreakOutputs = CONNECT_RESPONSE_BREAK_OTHERS_A;
		}
		else
		{
			ReplyBreakOutputs = CONNECT_RESPONSE_BREAK_OTHERS_B;
		}
		return FPinConnectionResponse(ReplyBreakOutputs, LOCTEXT("ConnectionReplace", "Replace existing connections"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UQuestGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	bool bModified = UEdGraphSchema::TryCreateConnection(PinA, PinB);

	if (bModified)
	{
		// CastChecked<UQuestGraph>(PinA->GetOwningNode()->GetGraph())->GetQuestObject()->CompileSoundNodesFromGraphNodes();
	}

	return bModified;
}

bool UQuestGraphSchema::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return true;
}

FLinearColor UQuestGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

void UQuestGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	Super::BreakNodeLinks(TargetNode);

	// CastChecked<UQuestGraph>(TargetNode.GetGraph())->GetQuestObject()->CompileSoundNodesFromGraphNodes();
}

void UQuestGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction( NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links") );

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);

	// if this would notify the node then we need to compile the SoundCue
	// if (bSendsNodeNotifcation)
	// {
	// 	CastChecked<UQuestGraph>(TargetPin.GetOwningNode()->GetGraph())->GetQuestObject()->CompileSoundNodesFromGraphNodes();
	// }
}

void UQuestGraphSchema::DroppedAssetsOnGraph(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const
{
	
}

void UQuestGraphSchema::GetAllQuestNodeActions(FGraphActionMenuBuilder& ActionMenuBuilder, bool bShowSelectedActions) const
{
	InitQuestNodeClasses();

	FText SelectedItemText;

	// if (bShowSelectedActions)
	// {
	// 	FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();
	//
	// 	// Get display text for any items that may be selected
	// 	if (ActionMenuBuilder.FromPin == nullptr)
	// 	{
	// 		TArray<UQuestObject*> SelectedQuests;
	// 		GEditor->GetSelectedObjects()->GetSelectedObjects<UQuestObject>(SelectedQuests);
	// 		if (SelectedQuests.Num())
	// 		{
	// 			SelectedItemText = FText::FromString(FString::Printf(TEXT("%s"), (SelectedQuests.Num() > 1) ? *(LOCTEXT("MultipleQuestsSelected", "Multiple Quests").ToString()) : *SelectedQuests[0]->GetName()));
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UQuestObject* SelectedQuest = GEditor->GetSelectedObjects()->GetTop<UQuestObject>();
	// 		if (SelectedQuest && ActionMenuBuilder.FromPin->Direction == EGPD_Input)
	// 		{
	// 			SelectedItemText = FText::FromString(SelectedWave->GetName());
	// 		}
	// 	}
	//
	// 	bShowSelectedActions = !SelectedItemText.IsEmpty();
	// }
	//
	// for (UClass* SoundNodeClass : QuestNodeClasses)
	// {
	// 	UQuestNode* QuestNode = SoundNodeClass->GetDefaultObject<UQuestNode>();
	//
	// 	// when dragging from an output pin you can create anything but a wave player
	// 	if (!ActionMenuBuilder.FromPin || ActionMenuBuilder.FromPin->Direction == EGPD_Input || QuestNode->GetMaxChildNodes() > 0)
	// 	{
	// 		const FText Name = FText::FromString(SoundNodeClass->GetDescription());
	//
	// 		{
	// 			FFormatNamedArguments Arguments;
	// 			Arguments.Add(TEXT("Name"), Name);
	// 			const FText AddToolTip = FText::Format(LOCTEXT("NewSoundCueNodeTooltip", "Adds {Name} node here"), Arguments);
	// 			TSharedPtr<FQuestGraphSchemaAction_NewNode> NewNodeAction(new FQuestGraphSchemaAction_NewNode(LOCTEXT("SoundNodeAction", "Sound Node"), Name, AddToolTip.ToString(), 0));
	// 			ActionMenuBuilder.AddAction(NewNodeAction);
	// 			NewNodeAction->QuestNodeClass = SoundNodeClass;
	// 		}
	//
	// 		if (bShowSelectedActions && (SoundNode->GetMaxChildNodes() == USoundNode::MAX_ALLOWED_CHILD_NODES || SoundNodeClass == UQuestNodeWavePlayer::StaticClass()))
	// 		{
	// 			FFormatNamedArguments Arguments;
	// 			Arguments.Add(TEXT("Name"), Name);
	// 			Arguments.Add(TEXT("SelectedItems"), SelectedItemText);
	// 			const FText MenuDesc = FText::Format(LOCTEXT("NewSoundNodeRandom", "{Name}: {SelectedItems}"), Arguments);
	// 			const FText ToolTip = FText::Format(LOCTEXT("NewSoundNodeRandomTooltip", "Adds a {Name} node for {SelectedItems} here"), Arguments);
	// 			TSharedPtr<FSoundCueGraphSchemaAction_NewFromSelected> NewNodeAction(new FSoundCueGraphSchemaAction_NewFromSelected(LOCTEXT("FromSelected", "From Selected"),
	// 				MenuDesc,
	// 				ToolTip.ToString(), 0));
	// 			ActionMenuBuilder.AddAction(NewNodeAction);
	// 			NewNodeAction->SoundNodeClass = (SoundNodeClass == USoundNodeWavePlayer::StaticClass() ? NULL : SoundNodeClass);
	// 		}
	// 	}
	// }
}

void UQuestGraphSchema::GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder, const UEdGraph* CurrentGraph) const
{
	// if (!ActionMenuBuilder.FromPin)
	// {
	// 	const bool bIsManyNodesSelected = CurrentGraph ? (FQuestEditorUtilities::GetNumberOfSelectedNodes(CurrentGraph) > 0) : false;
	// 	const FText MenuDescription = bIsManyNodesSelected ? LOCTEXT("CreateCommentAction", "Create Comment from Selection") : LOCTEXT("AddCommentAction", "Add Comment...");
	// 	const FString ToolTip = LOCTEXT("CreateCommentToolTip", "Creates a comment.").ToString();
	//
	// 	TSharedPtr<FQuestGraphSchemaAction_NewComment> NewAction(new FQuestGraphSchemaAction_NewComment(FText::GetEmpty(), MenuDescription, ToolTip, 0));
	// 	ActionMenuBuilder.AddAction( NewAction );
	// }
}

void UQuestGraphSchema::InitQuestNodeClasses()
{
	if(bQuestNodeClassesInitialized)
	{
		return;
	}

	// Construct list of non-abstract sound node classes.
	for(TObjectIterator<UClass> It; It; ++It)
	{
		if(It->IsChildOf(USoundNode::StaticClass()) 
			&& !It->HasAnyClassFlags(CLASS_Abstract))
		{
			QuestNodeClasses.Add(*It);
		}
	}

	QuestNodeClasses.Sort();

	bQuestNodeClassesInitialized = true;
}

int32 UQuestGraphSchema::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	// return FQuestEditorUtilities::GetNumberOfSelectedNodes(Graph);
    return INDEX_NONE;
}

TSharedPtr<FEdGraphSchemaAction> UQuestGraphSchema::GetCreateCommentAction() const
{
	return TSharedPtr<FEdGraphSchemaAction>(static_cast<FEdGraphSchemaAction*>(new FQuestGraphSchemaAction_NewComment));
}

#undef LOCTEXT_NAMESPACE