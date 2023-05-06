/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_QuestSystemSettingsDataTypes.h"
#include "RPG_QuestSystemSettings.generated.h"

class URPG_QuestObjectBase;

/**
 * @class Config of settings for the quest system
 */
UCLASS(Config = "Game", defaultconfig, meta = (DisplayName = "Quest System Settings"))
class RPG_QUESTSYSTEM_API URPG_QuestSystemSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    URPG_QuestSystemSettings();

    /** @public Configuring start node **/
    UPROPERTY(Config, EditAnywhere, Category = "Settings Editor Quest")
    FRPG_TaskNodeEditorColor StartTaskNodeEditorColor;

    /** @public Configuring standard nodes **/
    UPROPERTY(Config, EditAnywhere, Category = "Settings Editor Quest")
    FRPG_TaskNodeEditorColor StandardTaskNodeEditorColor;

    /** @public Configuring finish nodes **/
    UPROPERTY(Config, EditAnywhere, Category = "Settings Editor Quest")
    FRPG_TaskNodeEditorColor FinishTaskNodeEditorColor;

    /** @public Pin Color Settings **/
    UPROPERTY(Config, EditAnywhere, Category = "Settings Editor Quest")
    FRPG_SchemePinColor SchemePinColor;

    /** @public Distribution by category depending on the type of node **/
    UPROPERTY(Config, EditAnywhere, Category = "Settings Editor Quest")
    TMap<ERPG_TypeNode, FString> NameCategoryByTypeNode;

    UPROPERTY(Config, EditAnywhere, NoClear, Category = "Settings Editor Quest", meta = (ConfigRestartRequired = true))
    TSubclassOf<URPG_QuestObjectBase> SupportQuestClass;
};
