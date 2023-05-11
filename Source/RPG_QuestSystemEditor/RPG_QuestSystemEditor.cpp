/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestSystemEditor.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Actions/FRPG_QuestAssetTypeActions.h"
#include "AssetStyle/RPG_QuestSystemStyle.h"

#define LOCTEXT_NAMESPACE "FRPG_QuestSystemEditorModule"

EAssetTypeCategories::Type FRPG_QuestSystemEditorModule::AssetCategory = static_cast<EAssetTypeCategories::Type>(0);

void FRPG_QuestSystemEditorModule::StartupModule()
{
    RegisterAssetCategoryAndActions();
    FRPG_QuestSystemStyle::Initialize();
}

void FRPG_QuestSystemEditorModule::ShutdownModule()
{
    FRPG_QuestSystemStyle::Shutdown();
}

void FRPG_QuestSystemEditorModule::RegisterAssetCategoryAndActions() const
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("QuestSystemPlugin")), LOCTEXT("Quest", "Quest Plugin"));
    AssetTools.RegisterAssetTypeActions(MakeShareable(new FRPG_QuestAssetTypeActions()));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPG_QuestSystemEditorModule, RPG_QuestSystemEditor)