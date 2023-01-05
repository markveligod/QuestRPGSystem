
#include "QuestAssetTypeActions.h"
#include "QuestEditorModule.h"
#include "QuestRPGSystem/Objects/QuestObject.h"

UClass* FQuestAssetTypeActions::GetSupportedClass() const
{
    return UQuestObject::StaticClass();
}

void FQuestAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
    const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
    for (const auto Object : InObjects)
    {
        if (UQuestObject* NewAsset = Cast<UQuestObject>(Object))
        {
            IQuestEditorModule* QuestEditorModule = &FModuleManager::LoadModuleChecked<IQuestEditorModule>( "QuestSystemEditor" );
            QuestEditorModule->CreateQuestEditor(Mode, EditWithinLevelEditor, NewAsset);
        }
    }
}

FText FQuestAssetTypeActions::GetName() const
{
    return INVTEXT("Normal Distribution");
}

FColor FQuestAssetTypeActions::GetTypeColor() const
{
    return FColor::Cyan;
}

uint32 FQuestAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::Misc;
}
