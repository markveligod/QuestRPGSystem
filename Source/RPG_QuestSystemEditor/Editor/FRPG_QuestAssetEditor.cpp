/** Copyright Mark Veligod. Published in 2023. **/

#include "FRPG_QuestAssetEditor.h"
#include "EdGraphNode_Comment.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/RPG_QuestGraph.h"
#include "Graph/RPG_QuestGraphSchema.h"
#include "Graph/GraphNodes/RPG_QuestGraphNode_Base.h"
#include "Graph/Widgets/RPG_EditorPaletteTask.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "QuestAssetEditor"

const FName QuestEditorAppName = FName(TEXT("QuestEditorApp"));

struct FQuestEditorTabs
{
    // Tab identifiers
    static const FName QuestDetailsID;
    static const FName QuestViewportID;
    static const FName QuestListTaskID;
};

const FName FQuestEditorTabs::QuestDetailsID(TEXT("QuestDetails"));
const FName FQuestEditorTabs::QuestViewportID(TEXT("QuestViewport"));
const FName FQuestEditorTabs::QuestListTaskID(TEXT("QuestListTask"));

FRPG_QuestAssetEditor::FRPG_QuestAssetEditor()
{
    UEditorEngine* Editor = static_cast<UEditorEngine*>(GEngine);
    if (Editor != nullptr)
    {
        Editor->RegisterForUndo(this);
    }
}

FRPG_QuestAssetEditor::~FRPG_QuestAssetEditor()
{
    UEditorEngine* Editor = static_cast<UEditorEngine*>(GEngine);
    if (Editor)
    {
        Editor->UnregisterForUndo(this);
    }
}

void FRPG_QuestAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_DialogueEditor", "Dialogue Editor"));
    auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

    InTabManager->RegisterTabSpawner(FQuestEditorTabs::QuestViewportID, FOnSpawnTab::CreateSP(this, &FRPG_QuestAssetEditor::SpawnTab_Viewport))
        .SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports"));

    InTabManager->RegisterTabSpawner(FQuestEditorTabs::QuestDetailsID, FOnSpawnTab::CreateSP(this, &FRPG_QuestAssetEditor::SpawnTab_Details))
        .SetDisplayName(LOCTEXT("DetailsTabLabel", "Details"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

    InTabManager->RegisterTabSpawner(FQuestEditorTabs::QuestListTaskID, FOnSpawnTab::CreateSP(this, &FRPG_QuestAssetEditor::SpawnTab_ListTask))
        .SetDisplayName(LOCTEXT("LisTaskLabel", "ListTask"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Kismet.Tabs.Palette"));
}

void FRPG_QuestAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

    InTabManager->UnregisterTabSpawner(FQuestEditorTabs::QuestViewportID);
    InTabManager->UnregisterTabSpawner(FQuestEditorTabs::QuestDetailsID);
    InTabManager->UnregisterTabSpawner(FQuestEditorTabs::QuestListTaskID);
}

void FRPG_QuestAssetEditor::PostUndo(bool bSuccess)
{
    FEditorUndoClient::PostUndo(bSuccess);
}

void FRPG_QuestAssetEditor::PostRedo(bool bSuccess)
{
    FEditorUndoClient::PostRedo(bSuccess);
}

FName FRPG_QuestAssetEditor::GetToolkitFName() const
{
    return FName("QuestAssetEditor");
}

FText FRPG_QuestAssetEditor::GetBaseToolkitName() const
{
    return GetToolkitName();
}

FText FRPG_QuestAssetEditor::GetToolkitName() const
{
    return FText::FromString(QuestBeingEdited->GetName());
}

FText FRPG_QuestAssetEditor::GetToolkitToolTipText() const
{
    const UObject* EditingObject = QuestBeingEdited;

    check(EditingObject != NULL);

    return GetToolTipTextForObject(EditingObject);
}

FLinearColor FRPG_QuestAssetEditor::GetWorldCentricTabColorScale() const
{
    return FLinearColor::White;
}

FString FRPG_QuestAssetEditor::GetWorldCentricTabPrefix() const
{
    return TEXT("QuestAssetEditor");
}

bool FRPG_QuestAssetEditor::IsAssetEditor() const
{
    return FAssetEditorToolkit::IsAssetEditor();
}

bool FRPG_QuestAssetEditor::ProcessCommandBindings(const FKeyEvent& InKeyEvent) const
{
    return FAssetEditorToolkit::ProcessCommandBindings(InKeyEvent);
}

void FRPG_QuestAssetEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObject(QuestBeingEdited);
}

FString FRPG_QuestAssetEditor::GetReferencerName() const
{
    return QuestBeingEdited ? QuestBeingEdited->GetName() : FGCObject::GetReferencerName();
}

void FRPG_QuestAssetEditor::BindGraphCommands()
{
    const FGenericCommands& GenericCommands = FGenericCommands::Get();
    const FGraphEditorCommandsImpl& GraphCommands = FGraphEditorCommands::Get();

    ToolkitCommands->MapAction(
        GenericCommands.Delete, FExecuteAction::CreateSP(this, &FRPG_QuestAssetEditor::DeleteSelectedNodes), FCanExecuteAction::CreateSP(this, &FRPG_QuestAssetEditor::CanDeleteNodes));
}

void FRPG_QuestAssetEditor::InitQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, URPG_QuestObjectBase* InitQuestObject)
{
    QuestBeingEdited = InitQuestObject;
    TSharedPtr<FRPG_QuestAssetEditor> QuestEditorPtr = SharedThis(this);

    BindGraphCommands();
    // Default layout
    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout =
        FTabManager::NewLayout("Standalone_QuestEditor_Layout_v3")
            ->AddArea(FTabManager::NewPrimaryArea()
                          ->SetOrientation(Orient_Vertical)
                          ->Split(FTabManager::NewSplitter()
                                      ->SetOrientation(Orient_Horizontal)
                                      ->SetSizeCoefficient(0.9f)
                                      ->Split(FTabManager::NewStack()->SetSizeCoefficient(0.20f)->AddTab(FQuestEditorTabs::QuestDetailsID, ETabState::OpenedTab))
                                      ->Split(FTabManager::NewStack()->SetSizeCoefficient(0.50f)->AddTab(FQuestEditorTabs::QuestViewportID, ETabState::OpenedTab))
                                      ->Split(FTabManager::NewStack()->SetSizeCoefficient(0.20f)->AddTab(FQuestEditorTabs::QuestListTaskID, ETabState::OpenedTab))));

    // Initialize the asset editor
    InitAssetEditor(Mode, InitToolkitHost, QuestEditorAppName, StandaloneDefaultLayout, true, true, InitQuestObject);
    RegenerateMenusAndToolbars();
}

TSharedRef<SDockTab> FRPG_QuestAssetEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
    TSharedPtr<FRPG_QuestAssetEditor> QuestEditorPtr = SharedThis(this);

    TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab).Label(LOCTEXT("QuestGraphTitle", "Graph"));

    FocusedGraphEditor = CreateGraphWidget();
    if (FocusedGraphEditor.IsValid())
    {
        QuestEdGraph->GetSchema()->CreateDefaultNodesForGraph(*QuestEdGraph);
        SpawnedTab->SetContent(FocusedGraphEditor.ToSharedRef());
    }

    return SpawnedTab;
}

TSharedRef<SDockTab> FRPG_QuestAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
    TSharedPtr<FRPG_QuestAssetEditor> QuestEditorPtr = SharedThis(this);

    FDetailsViewArgs DetailsArgs;
    DetailsArgs.bHideSelectionTip = true;
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    QuestProperties = PropertyModule.CreateDetailView(DetailsArgs);

    // Spawn the tab
    return SNew(SDockTab).Label(LOCTEXT("DetailsTab_Title", "Details"))[QuestProperties.ToSharedRef()];
}

TSharedRef<SDockTab> FRPG_QuestAssetEditor::SpawnTab_ListTask(const FSpawnTabArgs& Args)
{
    Palette = SNew(SEditorPaletteTasks, SharedThis(this));

    return SNew(SDockTab)[Palette.ToSharedRef()];
}

TSharedRef<SGraphEditor> FRPG_QuestAssetEditor::CreateGraphWidget()
{
    SGraphEditor::FGraphEditorEvents InEvents;
    InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FRPG_QuestAssetEditor::OnSelectedNodesChanged);
    InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FRPG_QuestAssetEditor::OnNodeTitleCommitted);
    QuestEdGraph = Cast<URPG_QuestGraph>(FBlueprintEditorUtils::CreateNewGraph(QuestBeingEdited, "Quest Graph", URPG_QuestGraph::StaticClass(), URPG_QuestGraphSchema::StaticClass()));

    return SNew(SGraphEditor)
        .AdditionalCommands(ToolkitCommands)
        .IsEditable(true)
        .Appearance(GetGraphAppearanceInfo())
        .GraphToEdit(QuestEdGraph)
        .GraphEvents(InEvents)
        .AutoExpandActionMenu(true)
        .ShowGraphStateOverlay(false);
}

void FRPG_QuestAssetEditor::OnSelectedNodesChanged(const TSet<UObject*>& Nodes)
{
    TArray<UObject*> Selection;

    if (Nodes.Num() != 0)
    {
        for (TSet<class UObject*>::TConstIterator SetIt(Nodes); SetIt; ++SetIt)
        {
            if (URPG_QuestGraphNode_Base* GraphNodeBase = Cast<URPG_QuestGraphNode_Base>(*SetIt))
            {
                FRPG_TaskNodeData* TaskNodeElem = GraphNodeBase->GetTaskNodeData();
                if (TaskNodeElem && TaskNodeElem->TaskNodeBase)
                {
                    Selection.Add(TaskNodeElem->TaskNodeBase);
                }
            }
            if (UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(*SetIt))
            {
                Selection.Add(CommentNode);
            }
        }
        QuestProperties->SetObjects(Selection);
    }
    else
    {
        QuestProperties->SetObject(nullptr);
    }
}

void FRPG_QuestAssetEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{
    if (NodeBeingChanged)
    {
        const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Rename Node"));
        NodeBeingChanged->Modify();
        NodeBeingChanged->OnRenameNode(NewText.ToString());
    }
}

FGraphAppearanceInfo FRPG_QuestAssetEditor::GetGraphAppearanceInfo() const
{
    FGraphAppearanceInfo AppearanceInfo;
    AppearanceInfo.CornerText = GetCornerText();
    AppearanceInfo.InstructionText = GetInstructionText();

    return AppearanceInfo;
}

FText FRPG_QuestAssetEditor::GetCornerText() const
{
    return FText::FromString("QUEST");
}

FText FRPG_QuestAssetEditor::GetInstructionText() const
{
    return FText::FromString("Put the nodes from the list of tasks on the graph and connect them");
}

void FRPG_QuestAssetEditor::DeleteSelectedNodes()
{
    if (!QuestBeingEdited) return;
    const FScopedTransaction Transaction(LOCTEXT("DeleteSelectedNode", "Delete Selected Node"));
    FocusedGraphEditor->GetCurrentGraph()->Modify();

    const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEditor->GetSelectedNodes();

    for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
    {
        UEdGraphNode* Node = CastChecked<UEdGraphNode>(*NodeIt);

        if (Node && Node->CanUserDeleteNode())
        {
            if (const URPG_QuestGraphNode_Base* QuestGraphNode = Cast<URPG_QuestGraphNode_Base>(Node))
            {
                FRPG_TaskNodeData* TaskNodeElem = QuestGraphNode->GetTaskNodeData();
                if (TaskNodeElem)
                {
                    QuestBeingEdited->RemoveTaskNode(TaskNodeElem->IndexNode);
                }
            }
            FBlueprintEditorUtils::RemoveNode(nullptr, Node, true);
        }
    }
}

bool FRPG_QuestAssetEditor::CanDeleteNodes() const
{
    const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEditor->GetSelectedNodes();
    for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
    {
        if (const URPG_QuestGraphNode_Base* Node = Cast<URPG_QuestGraphNode_Base>(*NodeIt))
        {
            if (Node->GetTypeTaskNode() == ERPG_TypeNode::StartNode)
            {
                return false;
            }
        }
    }

    return SelectedNodes.Num() > 0;
}

#undef LOCTEXT_NAMESPACE
