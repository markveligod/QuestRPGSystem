/** Copyright Mark Veligod. Published in 2023. **/

#include "FRPG_QuestAssetTypeActions.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectBase.h"
#include "RPG_QuestSystemEditor/RPG_QuestSystemEditor.h"
#include "RPG_QuestSystemEditor/Editor/FRPG_QuestAssetEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FRPG_QuestAssetTypeActions::FRPG_QuestAssetTypeActions()
{
}

FText FRPG_QuestAssetTypeActions::GetName() const
{
    return LOCTEXT("FQuestAssetTypeActionsName", "Quest");
}

FColor FRPG_QuestAssetTypeActions::GetTypeColor() const
{
    return FColor(255, 55, 220);
}

UClass* FRPG_QuestAssetTypeActions::GetSupportedClass() const
{
    return URPG_QuestObjectBase::StaticClass();
}

void FRPG_QuestAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
    const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

    for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
    {
        if (URPG_QuestObjectBase* Quest = Cast<URPG_QuestObjectBase>(*ObjIt))
        {
            TSharedRef<FRPG_QuestAssetEditor> NewQuestEditor(new FRPG_QuestAssetEditor());
            NewQuestEditor->InitQuestEditor(Mode, EditWithinLevelEditor, Quest);
        }
    }
}

uint32 FRPG_QuestAssetTypeActions::GetCategories()
{
    return FRPG_QuestSystemEditorModule::AssetCategory;
}

#undef LOCTEXT_NAMESPACE