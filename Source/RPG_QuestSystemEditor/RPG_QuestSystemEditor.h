/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "Modules/ModuleManager.h"

class FRPG_QuestSystemEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;


#pragma region Action
    
protected:

    /** @private Creating a new category for the quest system **/
    void RegisterAssetCategoryAndActions() const;

#pragma endregion

#pragma region Data

public:

    static EAssetTypeCategories::Type AssetCategory;

#pragma endregion
};
