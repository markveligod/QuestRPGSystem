
#include "QuestEditorUtilities.h"
#include "QuestRPGSystem/Objects/QuestObject.h"
#include "QuestSystemEditor/IQuestEditor.h"
#include "QuestSystemEditor/QuestGraph/QuestGraph.h"
#include "Toolkits/ToolkitManager.h"
#include "Toolkits/IToolkit.h"

bool FQuestEditorUtilities::CanPasteNodes(const UEdGraph* Graph)
{
    bool bCanPaste = false;
    const TSharedPtr<IQuestEditor> QuestEditor = GetIQuestEditorForObject(Graph);
    if(QuestEditor.IsValid())
    {
        bCanPaste = QuestEditor->CanPasteNodes();
    }
    return bCanPaste;
}

void FQuestEditorUtilities::PasteNodesHere(const UEdGraph* Graph, const FVector2D& Location)
{
    const TSharedPtr<IQuestEditor> QuestEditor = GetIQuestEditorForObject(Graph);
    if(QuestEditor.IsValid())
    {
        QuestEditor->PasteNodesHere(Location);
    }
}

TSharedPtr<IQuestEditor> FQuestEditorUtilities::GetIQuestEditorForObject(const UObject* ObjectToFocusOn)
{
    check(ObjectToFocusOn);

    // Find the associated SoundCue
    const UQuestObject* QuestObject = Cast<const UQuestGraph>(ObjectToFocusOn)->GetQuestObject();

    TSharedPtr<IQuestEditor> QuestEditor;
    if (QuestObject)
    {
        const TSharedPtr< IToolkit > FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(QuestObject);
        if (FoundAssetEditor.IsValid())
        {
            QuestEditor = StaticCastSharedPtr<IQuestEditor>(FoundAssetEditor);
        }
    }
    return QuestEditor;
}

bool FQuestEditorUtilities::GetBoundsForSelectedNodes(const UEdGraph* Graph, FSlateRect& Rect, float Padding)
{
    const TSharedPtr<IQuestEditor> QuestEditor = GetIQuestEditorForObject(Graph);
    if(QuestEditor.IsValid())
    {
        return QuestEditor->GetBoundsForSelectedNodes(Rect, Padding);
    }
    return false;
}

int32 FQuestEditorUtilities::GetNumberOfSelectedNodes(const UEdGraph* Graph)
{
    const TSharedPtr<IQuestEditor> QuestEditor = GetIQuestEditorForObject(Graph);
    if(QuestEditor.IsValid())
    {
        return QuestEditor->GetNumberOfSelectedNodes();
    }
    return 0;
}

TSet<UObject*> FQuestEditorUtilities::GetSelectedNodes(const UEdGraph* Graph)
{
    const TSharedPtr<IQuestEditor> QuestEditor = GetIQuestEditorForObject(Graph);
    if(QuestEditor.IsValid())
    {
        return QuestEditor->GetSelectedNodes();
    }
    return TSet<UObject*>();
}
