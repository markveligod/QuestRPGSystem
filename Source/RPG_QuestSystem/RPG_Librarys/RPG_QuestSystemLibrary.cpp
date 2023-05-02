/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestSystemLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<bool> EnableD_QuestSystemLog(TEXT("QuestSystem.ShowLog"), false, TEXT("Enable logging in the quest system"), ECVF_Cheat);
#endif

void URPG_QuestSystemLibrary::Print_Log(const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function)
{
    
#if !UE_EDITOR && (UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG)
    if (!EnableD_QuestSystemLog.GetValueOnGameThread()) return;
#endif

    if (LogVerb == Display)
    {
        UE_LOG(LogQuestSystem, Display, TEXT("[%s][%d] TEXT:[%s]"), ANSI_TO_TCHAR(Function), Line, *Text);
    }
    else if (LogVerb == Warning)
    {
        UE_LOG(LogQuestSystem, Warning, TEXT("[%s][%d] TEXT:[%s]"), ANSI_TO_TCHAR(Function), Line, *Text);
    }
    else if (LogVerb == Error)
    {
        UE_LOG(LogQuestSystem, Error, TEXT("[%s][%d] TEXT:[%s]"), ANSI_TO_TCHAR(Function), Line, *Text);
    }
}

bool URPG_QuestSystemLibrary::Print_CLog(const bool bCond, const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function)
{
    if (bCond)
    {
        URPG_QuestSystemLibrary::Print_Log(LogVerb, Text, Line, Function);
    }
    return bCond;
}

FString URPG_QuestSystemLibrary::GetNetModeToString(AActor* InActor)
{
    if (!InActor) return FString("NM_Client");

    const ENetMode NetMode = InActor->GetNetMode();

    switch (NetMode)
    {
        case NM_Standalone: return FString("NM_Standalone");
        case NM_DedicatedServer: return FString("NM_DedicatedServer");
        case NM_Client: return FString("NM_Client");
        case NM_ListenServer: return FString("NM_ListenServer");
    }
    return FString("nullptr");
}
