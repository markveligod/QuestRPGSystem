
#include "QuestEditor.h"
#include "QuestEditorModule.h"
#include "ScopedTransaction.h"
#include "Toolkits/IToolkitHost.h"
#include "GraphEditor.h"
#include "GraphEditorActions.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "SoundCueGraphEditorCommands.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/IDetailsView.h"
#include "Framework/Commands/GenericCommands.h"
#include "QuestGraph/QuestEdGraphSchema.h"
#include "QuestGraph/QuestGraph.h"
#include "QuestGraph/QuestGraphEditorCommands.h"
#include "QuestGraph/QuestGraphNode.h"

#define LOCTEXT_NAMESPACE "QuestEditor"

const FName FQuestEditor::GraphCanvasTabId( TEXT( "QuestEditor_GraphCanvas" ) );
const FName FQuestEditor::PropertiesTabId( TEXT( "QuestEditor_Properties" ) );

void FQuestEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SoundCueEditor", "Sound Cue Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner( GraphCanvasTabId, FOnSpawnTab::CreateSP(this, &FQuestEditor::SpawnTab_GraphCanvas) )
		.SetDisplayName( LOCTEXT("GraphCanvasTab", "Viewport") )
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner( PropertiesTabId, FOnSpawnTab::CreateSP(this, &FQuestEditor::SpawnTab_Properties) )
		.SetDisplayName( LOCTEXT("DetailsTab", "Details") )
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FQuestEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner( GraphCanvasTabId );
	InTabManager->UnregisterTabSpawner( PropertiesTabId );
}

FQuestEditor::~FQuestEditor()
{
	GEditor->UnregisterForUndo( this );
}

void FQuestEditor::InitQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit)
{
	QuestObject = CastChecked<UQuestObject>(ObjectToEdit);

	// Support undo/redo
	QuestObject->SetFlags(RF_Transactional);

	GEditor->RegisterForUndo(this);

	FGraphEditorCommands::Register();
	FQuestGraphEditorCommands::Register();

	BindGraphCommands();
	CreateInternalWidgets();

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_QuestEditor_Layout_v3")
	->AddArea
	(
		FTabManager::NewPrimaryArea() ->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter() ->SetOrientation(Orient_Horizontal) ->SetSizeCoefficient(0.9f)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.225f)
				->AddTab(PropertiesTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.65f)
				->AddTab(GraphCanvasTabId, ETabState::OpenedTab) ->SetHideTabWell( true )
			)
		)
	);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode, InitToolkitHost, QuestEditorAppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectToEdit, false);

	IQuestEditorModule* QuestEditorModule = &FModuleManager::LoadModuleChecked<IQuestEditorModule>( "QuestSystemEditor" );
	AddMenuExtender(QuestEditorModule->GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

	RegenerateMenusAndToolbars();

	// @todo toolkit world centric editing
	/*if(IsWorldCentricAssetEditor())
	{
		SpawnToolkitTab(GetToolbarTabId(), FString(), EToolkitTabSpot::ToolBar);
		SpawnToolkitTab(GraphCanvasTabId, FString(), EToolkitTabSpot::Viewport);
		SpawnToolkitTab(PropertiesTabId, FString(), EToolkitTabSpot::Details);
	}*/
}

UQuestObject* FQuestEditor::GetQuestObject() const
{
    return QuestObject;
}

void FQuestEditor::SetSelection(TArray<UObject*> SelectedObjects)
{
	if (QuestProperties.IsValid())
	{
		QuestProperties->SetObjects(SelectedObjects);
	}
}

bool FQuestEditor::GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding )
{
	return QuestGraphEditor->GetBoundsForSelectedNodes(Rect, Padding);
}

int32 FQuestEditor::GetNumberOfSelectedNodes() const
{
	return QuestGraphEditor->GetSelectedNodes().Num();
}

FName FQuestEditor::GetToolkitFName() const
{
	return FName("QuestEditor");
}

FText FQuestEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Quest Editor");
}

FString FQuestEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Quest ").ToString();
}

FLinearColor FQuestEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

TSharedRef<SDockTab> FQuestEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
	check( Args.GetTabId() == GraphCanvasTabId );

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("SoundCueGraphCanvasTitle", "Viewport"));

	if (QuestGraphEditor.IsValid())
	{
		SpawnedTab->SetContent(QuestGraphEditor.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FQuestEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check( Args.GetTabId() == PropertiesTabId );

	return SNew(SDockTab)
		.Icon(FAppStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("QuestDetailsTitle", "Details"))
		[
			QuestProperties.ToSharedRef()
		];
}

void FQuestEditor::OnPreviewTextChanged(const FString& Text)
{
    
}

void FQuestEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(QuestObject);
}

void FQuestEditor::PostUndo(bool bSuccess)
{
	if (QuestGraphEditor.IsValid())
	{
		QuestGraphEditor->ClearSelectionSet();
		QuestGraphEditor->NotifyGraphChanged();
	}

}

void FQuestEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (QuestGraphEditor.IsValid() && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		QuestGraphEditor->NotifyGraphChanged();
	}
}

void FQuestEditor::CreateInternalWidgets()
{
	QuestGraphEditor = CreateGraphEditorWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	QuestProperties = PropertyModule.CreateDetailView(Args);
	QuestProperties->SetObject(QuestObject);

    if (QuestEdGraph)
    {
        CastChecked<UQuestGraphSchema>(QuestEdGraph->GetSchema())->CreateDefaultNodesForGraph(QuestEdGraph);
    }
}

void FQuestEditor::BindGraphCommands()
{
	ToolkitCommands->MapAction(
		FGenericCommands::Get().Undo,
		FExecuteAction::CreateSP( this, &FQuestEditor::UndoGraphAction ));

	ToolkitCommands->MapAction(
		FGenericCommands::Get().Redo,
		FExecuteAction::CreateSP( this, &FQuestEditor::RedoGraphAction ));
}

void FQuestEditor::AddInput()
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	// Iterator used but should only contain one node
	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UQuestGraphNode* SelectedNode = Cast<UQuestGraphNode>(*NodeIt);

		if (SelectedNode)
		{
			SelectedNode->AddInputPin();
			break;
		}
	}
}

bool FQuestEditor::CanAddInput() const
{
	return GetSelectedNodes().Num() == 1;
}

void FQuestEditor::DeleteInput()
{
	UEdGraphPin* SelectedPin = QuestGraphEditor->GetGraphPinForMenu();
	
	UQuestGraphNode* SelectedNode = Cast<UQuestGraphNode>(SelectedPin->GetOwningNode());

	if (SelectedNode && SelectedNode == SelectedPin->GetOwningNode())
	{
		SelectedNode->RemoveInputPin(SelectedPin);
	}
}

bool FQuestEditor::CanDeleteInput() const
{
	return true;
}

void FQuestEditor::OnCreateComment()
{
	FQuestGraphSchemaAction_NewComment CommentAction;
	CommentAction.PerformAction(QuestEdGraph, nullptr, QuestGraphEditor->GetPasteLocation());
}

TSharedRef<SGraphEditor> FQuestEditor::CreateGraphEditorWidget()
{
	if ( !GraphEditorCommands.IsValid() )
	{
		GraphEditorCommands = MakeShareable( new FUICommandList );

		GraphEditorCommands->MapAction( FQuestGraphEditorCommands::Get().AddInput,
			FExecuteAction::CreateSP(this, &FQuestEditor::AddInput),
			FCanExecuteAction::CreateSP( this, &FQuestEditor::CanAddInput ));
		
		GraphEditorCommands->MapAction( FQuestGraphEditorCommands::Get().DeleteInput,
			FExecuteAction::CreateSP(this, &FQuestEditor::DeleteInput),
			FCanExecuteAction::CreateSP( this, &FQuestEditor::CanDeleteInput ));

		// Graph Editor Commands
		GraphEditorCommands->MapAction( FGraphEditorCommands::Get().CreateComment,
			FExecuteAction::CreateSP( this, &FQuestEditor::OnCreateComment )
			);

		// Editing commands
		GraphEditorCommands->MapAction( FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateSP( this, &FQuestEditor::SelectAllNodes ),
			FCanExecuteAction::CreateSP( this, &FQuestEditor::CanSelectAllNodes )
			);

		GraphEditorCommands->MapAction( FGenericCommands::Get().Delete,
			FExecuteAction::CreateSP( this, &FQuestEditor::DeleteSelectedNodes ),
			FCanExecuteAction::CreateSP( this, &FQuestEditor::CanDeleteNodes )
			);

		GraphEditorCommands->MapAction( FGenericCommands::Get().Copy,
			FExecuteAction::CreateSP( this, &FQuestEditor::CopySelectedNodes ),
			FCanExecuteAction::CreateSP( this, &FQuestEditor::CanCopyNodes )
			);

		GraphEditorCommands->MapAction( FGenericCommands::Get().Cut,
			FExecuteAction::CreateSP( this, &FQuestEditor::CutSelectedNodes ),
			FCanExecuteAction::CreateSP( this, &FQuestEditor::CanCutNodes )
			);

		GraphEditorCommands->MapAction( FGenericCommands::Get().Paste,
			FExecuteAction::CreateSP( this, &FQuestEditor::PasteNodes ),
			FCanExecuteAction::CreateSP( this, &FQuestEditor::CanPasteNodes )
			);

		GraphEditorCommands->MapAction( FGenericCommands::Get().Duplicate,
			FExecuteAction::CreateSP( this, &FQuestEditor::DuplicateNodes ),
			FCanExecuteAction::CreateSP( this, &FQuestEditor::CanDuplicateNodes )
			);
	}

	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_Quest", "QUEST");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FQuestEditor::OnSelectedNodesChanged);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FQuestEditor::OnNodeTitleCommitted);
    QuestEdGraph = Cast<UQuestGraph>(FBlueprintEditorUtils::CreateNewGraph(QuestObject, "Quest Graph", UQuestGraph::StaticClass(), UQuestGraphSchema::StaticClass()));
    
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(QuestEdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

FGraphPanelSelectionSet FQuestEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (QuestGraphEditor.IsValid())
	{
		CurrentSelection = QuestGraphEditor->GetSelectedNodes();
	}
	return CurrentSelection;
}

void FQuestEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	if(NewSelection.Num() != 0)
	{
		for(TSet<class UObject*>::TConstIterator SetIt(NewSelection); SetIt; ++SetIt)
		{
			if (UQuestGraphNode* GraphNode = Cast<UQuestGraphNode>(*SetIt))
			{
				Selection.Add(GraphNode);
			}
		}
	    SetSelection(Selection);
	}
}

void FQuestEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{
	if (NodeBeingChanged)
	{
		const FScopedTransaction Transaction( LOCTEXT( "RenameNode", "Rename Node" ) );
		NodeBeingChanged->Modify();
		NodeBeingChanged->OnRenameNode(NewText.ToString());
	}
}

void FQuestEditor::SelectAllNodes()
{
	QuestGraphEditor->SelectAllNodes();
}

bool FQuestEditor::CanSelectAllNodes() const
{
	return true;
}

void FQuestEditor::DeleteSelectedNodes()
{
	const FScopedTransaction Transaction( NSLOCTEXT("UnrealEd", "SoundCueEditorDeleteSelectedNode", "Delete Selected Sound Cue Node") );

	QuestGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	QuestGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = CastChecked<UEdGraphNode>(*NodeIt);

		if (Node->CanUserDeleteNode())
		{
			if (UQuestGraphNode* SoundGraphNode = Cast<UQuestGraphNode>(Node))
			{
				// UQuestNode* DelNode = SoundGraphNode->QuestNode;
				//
				// FBlueprintEditorUtils::RemoveNode(NULL, SoundGraphNode, true);
				//
				// // Make sure SoundCue is updated to match graph
				// QuestObject->CompileSoundNodesFromGraphNodes();
				//
				// // Remove this node from the SoundCue's list of all SoundNodes
				// QuestObject->AllNodes.Remove(DelNode);
				// QuestObject->MarkPackageDirty();
			}
			else
			{
				FBlueprintEditorUtils::RemoveNode(nullptr, Node, true);
			}
		}
	}
}

bool FQuestEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	// if (SelectedNodes.Num() == 1)
	// {
	// 	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	// 	{
	// 		if (Cast<UQuestGraphNode_Root>(*NodeIt))
	// 		{
	// 			// Return false if only root node is selected, as it can't be deleted
	// 			return false;
	// 		}
	// 	}
	// }

	return SelectedNodes.Num() > 0;
}

void FQuestEditor::DeleteSelectedDuplicatableNodes()
{
	// Cache off the old selection
	const FGraphPanelSelectionSet OldSelectedNodes = GetSelectedNodes();

	// Clear the selection and only select the nodes that can be duplicated
	FGraphPanelSelectionSet RemainingNodes;
	QuestGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if ((Node != NULL) && Node->CanDuplicateNode())
		{
			QuestGraphEditor->SetNodeSelection(Node, true);
		}
		else
		{
			RemainingNodes.Add(Node);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	// Reselect whatever's left from the original selection after the deletion
	QuestGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(RemainingNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			QuestGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FQuestEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	// Cut should only delete nodes that can be duplicated
	DeleteSelectedDuplicatableNodes();
}

bool FQuestEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FQuestEditor::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if(UQuestGraphNode* Node = Cast<UQuestGraphNode>(*SelectedIter))
		{
			Node->PrepareForCopying();
		}
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
	FPlatformMisc::ClipboardCopy(*ExportedText);

	// Make sure SoundCue remains the owner of the copied nodes
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UQuestGraphNode* Node = Cast<UQuestGraphNode>(*SelectedIter))
		{
			Node->PostCopyNode();
		}
	}
}

bool FQuestEditor::CanCopyNodes() const
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if ((Node != NULL) && Node->CanDuplicateNode())
		{
			return true;
		}
	}
	return false;
}

void FQuestEditor::PasteNodes()
{
	PasteNodesHere(QuestGraphEditor->GetPasteLocation());
}

void FQuestEditor::PasteNodesHere(const FVector2D& Location)
{
	// // Undo/Redo support
	// const FScopedTransaction Transaction( NSLOCTEXT("UnrealEd", "SoundCueEditorPaste", "Paste Sound Cue Node") );
	// QuestObject->GetGraph()->Modify();
	// QuestObject->Modify();
	//
	// // Clear the selection set (newly pasted stuff will be selected)
	// QuestGraphEditor->ClearSelectionSet();
	//
	// // Grab the text to paste from the clipboard.
	// FString TextToImport;
	// FPlatformMisc::ClipboardPaste(TextToImport);
	//
	// // Import the nodes
	// TSet<UEdGraphNode*> PastedNodes;
	// FEdGraphUtilities::ImportNodesFromText(QuestObject->GetGraph(), TextToImport, /*out*/ PastedNodes);
	//
	// //Average position of nodes so we can move them while still maintaining relative distances to each other
	// FVector2D AvgNodePosition(0.0f,0.0f);
	//
	// for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	// {
	// 	UEdGraphNode* Node = *It;
	// 	AvgNodePosition.X += Node->NodePosX;
	// 	AvgNodePosition.Y += Node->NodePosY;
	// }
	//
	// if ( PastedNodes.Num() > 0 )
	// {
	// 	float InvNumNodes = 1.0f/float(PastedNodes.Num());
	// 	AvgNodePosition.X *= InvNumNodes;
	// 	AvgNodePosition.Y *= InvNumNodes;
	// }
	//
	// for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	// {
	// 	UEdGraphNode* Node = *It;
	//
	// 	if (UQuestGraphNode* SoundGraphNode = Cast<UQuestGraphNode>(Node))
	// 	{
	// 		QuestObject->AllNodes.Add(SoundGraphNode->SoundNode);
	// 	}
	//
	// 	// Select the newly pasted stuff
	// 	QuestGraphEditor->SetNodeSelection(Node, true);
	//
	// 	Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X ;
	// 	Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y ;
	//
	// 	Node->SnapToGrid(SNodePanel::GetSnapGridSize());
	//
	// 	// Give new node a different Guid from the old one
	// 	Node->CreateNewGuid();
	// }
	//
	// // Force new pasted SoundNodes to have same connections as graph nodes
	// QuestObject->CompileSoundNodesFromGraphNodes();
	//
	// // Update UI
	// QuestGraphEditor->NotifyGraphChanged();
	//
	// QuestObject->PostEditChange();
	// QuestObject->MarkPackageDirty();
}

bool FQuestEditor::CanPasteNodes() const
{
	// FString ClipboardContent;
	// FPlatformMisc::ClipboardPaste(ClipboardContent);
	//
	// return FEdGraphUtilities::CanImportNodesFromText(QuestObject->QuestGraph, ClipboardContent);
    return false;
}

void FQuestEditor::DuplicateNodes()
{
	// Copy and paste current selection
	CopySelectedNodes();
	PasteNodes();
}

bool FQuestEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FQuestEditor::UndoGraphAction()
{
	GEditor->UndoTransaction();
}

void FQuestEditor::RedoGraphAction()
{
	// Clear selection, to avoid holding refs to nodes that go away
	QuestGraphEditor->ClearSelectionSet();

	GEditor->RedoTransaction();
}

#undef LOCTEXT_NAMESPACE