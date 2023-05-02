/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_QuestSystemDataTypes.h"
#include "RPG_QuestSystemLibrary.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogQuestSystem, All, All);

/**
 * @class System library of quests for additional work of all processes
 */
UCLASS()
class RPG_QUESTSYSTEM_API URPG_QuestSystemLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** @public Write a log **/
    static void Print_Log(const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function);

    /** @public Write a clog **/
    static bool Print_CLog(const bool bCond, const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function);

    /** @public Net mode to string **/
    UFUNCTION(BlueprintCallable)
    static FString GetNetModeToString(AActor* InActor);
};
