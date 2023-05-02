/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "Toolkits/AssetEditorToolkit.h"
#include "EditorUndoClient.h"
#include "Framework/Commands/UICommandList.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectBase.h"

class URPG_QuestGraphSchema;
class URPG_QuestGraph;

class FRPG_QuestAssetEditor : public FAssetEditorToolkit, public FEditorUndoClient, FGCObject
{
public:
    FRPG_QuestAssetEditor();
    virtual ~FRPG_QuestAssetEditor();

    // IToolkit interface
    virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
    // End of IToolkit interface

    //~ Begin FEditorUndoClient Interface
    virtual void PostUndo(bool bSuccess) override;
    virtual void PostRedo(bool bSuccess) override;
    // End of FEditorUndoClient

    // FAssetEditorToolkit
    virtual FName GetToolkitFName() const override;				// Must implement in derived class!
    virtual FText GetBaseToolkitName() const override;			// Must implement in derived class!
    virtual FText GetToolkitName() const override;
    virtual FText GetToolkitToolTipText() const override;

    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual bool IsAssetEditor() const override;
    virtual bool ProcessCommandBindings(const FKeyEvent& InKeyEvent) const override;
    // End of FAssetEditorToolkit

    // FSerializableObject interface
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;
    // End of FSerializableObject interface

    virtual void BindGraphCommands();
    
public:
    void InitQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, URPG_QuestObjectBase* InitQuestObject);

    URPG_QuestObjectBase* GetQuestBeingEdited() const { return QuestBeingEdited; }
    
protected:

    TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_ListTask(const FSpawnTabArgs& Args);
    TSharedRef<SGraphEditor> CreateGraphWidget();

    void OnSelectedNodesChanged(const TSet<UObject*>& Nodes);
    void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);

    FGraphAppearanceInfo GetGraphAppearanceInfo() const;
    FText GetCornerText() const;
    FText GetInstructionText() const;

    virtual void DeleteSelectedNodes();
    virtual bool CanDeleteNodes() const;

private:
    /** The object we're currently editing */
    URPG_QuestObjectBase* QuestBeingEdited{nullptr};
    /** Properties tab */
    TSharedPtr<class IDetailsView> QuestProperties;

    TSharedPtr<SGraphEditor> FocusedGraphEditor;
    TSharedPtr<class SEditorPaletteTasks> Palette;
    URPG_QuestGraph* QuestEdGraph{nullptr};
};
