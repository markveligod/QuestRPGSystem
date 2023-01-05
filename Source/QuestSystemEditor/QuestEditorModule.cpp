
#include "QuestEditorModule.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "QuestEditor.h"

DEFINE_LOG_CATEGORY(LogQuestEditor);

const FName QuestEditorAppIdentifier = FName(TEXT("QuestEditorApp"));

/*-----------------------------------------------------------------------------
   FQuestEditorModule
-----------------------------------------------------------------------------*/

class FQuestEditorModule : public IQuestEditorModule
{
public:
	/** Constructor, set up console commands and variables **/
	FQuestEditorModule()
	{
	}

	/** Called right after the module DLL has been loaded and the module object has been created */
	virtual void StartupModule() override
	{
	    QuestAssetTypeActions = MakeShared<FQuestAssetTypeActions>();
	    FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(QuestAssetTypeActions.ToSharedRef());
	    
		MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
		ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);
	}

	/** Called before the module is unloaded, right before the module object is destroyed. */
	virtual void ShutdownModule() override
	{
	    if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	    FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(QuestAssetTypeActions.ToSharedRef());

		MenuExtensibilityManager.Reset();
		ToolBarExtensibilityManager.Reset();
	}

	/** Creates a new material editor, either for a material or a material function */
	virtual TSharedRef<IQuestEditor> CreateQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UQuestObject* QuestObject) override
	{
		TSharedRef<FQuestEditor> NewQuestEditor(new FQuestEditor());
		NewQuestEditor->InitQuestEditor(Mode, InitToolkitHost, QuestObject);
		return NewQuestEditor;
	}

	/** Gets the extensibility managers for outside entities to extend static mesh editor's menus and toolbars */
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
    TSharedPtr<FQuestAssetTypeActions> QuestAssetTypeActions;
};

IMPLEMENT_MODULE(FQuestEditorModule, QuestSystemEditor);
