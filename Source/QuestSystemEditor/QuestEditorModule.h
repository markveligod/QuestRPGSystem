#pragma once

#include "UnrealEd.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Toolkits/IToolkit.h"
#include "IQuestEditor.h"
#include "QuestAssetTypeActions.h"

extern const FName QuestEditorAppIdentifier;

DECLARE_LOG_CATEGORY_EXTERN(LogQuestEditor, Log, All);

/*-----------------------------------------------------------------------------
   IQuestEditorModule
-----------------------------------------------------------------------------*/

class IQuestEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
    
    /** Creates a new quest editor */
    virtual TSharedRef<IQuestEditor> CreateQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UQuestObject* Quest) = 0;

};