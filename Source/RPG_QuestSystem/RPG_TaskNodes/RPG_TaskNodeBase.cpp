/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_Librarys/RPG_QuestSystemLibrary.h"
#include "RPG_QuestObjects/RPG_QuestObjectBase.h"

#pragma region Log

void URPG_TaskNodeBase::Print_LogTask(const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const
{
    const FString NetMode = URPG_QuestSystemLibrary::GetNetModeToString(OwnerPC);
    const FString Result = FString::Printf(TEXT("NetMode: [%s] | Name task node: [%s] | Text: [%s]"), *NetMode, *GetName(), *Text);
    URPG_QuestSystemLibrary::Print_Log(LogVerb, Result, Line, Function);
}

bool URPG_TaskNodeBase::Print_CLogTask(const bool bCond, const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const
{
    const FString NetMode = URPG_QuestSystemLibrary::GetNetModeToString(OwnerPC);
    const FString Result = FString::Printf(TEXT("NetMode: [%s] | Name task node: [%s] | Text: [%s]"), *NetMode, *GetName(), *Text);
    return URPG_QuestSystemLibrary::Print_CLog(bCond, LogVerb, Result, Line, Function);
}

#pragma endregion

#pragma region DefaultInterface

URPG_TaskNodeBase::URPG_TaskNodeBase()
{

}

bool URPG_TaskNodeBase::InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest)
{
    if (TASK_NODE_CLOG(StateTaskNode != ERPG_StateEntity::None, Error, FString::Printf(TEXT("Task is not state NONE | Task current state: [%s]"), *UEnum::GetValueAsString(StateTaskNode))))
        return false;

    if (TASK_NODE_CLOG(!IsValid(PlayerController), Error, TEXT("Player Controller is nullptr"))) return false;
    OwnerPC = PlayerController;

    if (TASK_NODE_CLOG(!IsValid(ParentQuest), Error, TEXT("Parent Quest is nullptr"))) return false;
    OwnerQuestObject = ParentQuest;

    ChangeStateTaskNode(ERPG_StateEntity::Init);
    return InitTask_Event();
}

bool URPG_TaskNodeBase::InitTask_Event_Implementation()
{
    return true;
}

void URPG_TaskNodeBase::RunTask()
{
    if (TASK_NODE_CLOG(StateTaskNode != ERPG_StateEntity::Init, Error, FString::Printf(TEXT("Task is not state INIT | Task current state: [%s]"), *UEnum::GetValueAsString(StateTaskNode))))
        return;

    ChangeStateTaskNode(ERPG_StateEntity::Run);
    return RunTask_Event();
}

void URPG_TaskNodeBase::RunTask_Event_Implementation()
{
    
}

void URPG_TaskNodeBase::CompleteTask()
{
    if (TASK_NODE_CLOG(StateTaskNode != ERPG_StateEntity::Run, Error, FString::Printf(TEXT("Task is not state RUN | Task current state: [%s]"), *UEnum::GetValueAsString(StateTaskNode)))) return;

    ChangeStateTaskNode(ERPG_StateEntity::Complete);
    return CompleteTask_Event();
}

void URPG_TaskNodeBase::CompleteTask_Event_Implementation()
{
    
}

void URPG_TaskNodeBase::ResetTask()
{
    ChangeStateTaskNode(ERPG_StateEntity::None);
    return ResetTask_Event();
}

void URPG_TaskNodeBase::ResetTask_Event_Implementation()
{

}

TArray<URPG_TaskNodeBase*> URPG_TaskNodeBase::GetAllInstanced()
{
    return {this};
}

#pragma endregion

#pragma region NetworkInterface

int32 URPG_TaskNodeBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    return (OwnerPC ? OwnerPC->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool URPG_TaskNodeBase::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
    if (!OwnerPC && GetWorld()->GetNetMode() == NM_Client)
    {
        OwnerPC = GetWorld()->GetFirstPlayerController();
    }
    if (!OwnerPC) return false;
    UNetDriver* NetDriver = OwnerPC->GetNetDriver();
    if (!NetDriver) return false;
    NetDriver->ProcessRemoteFunction(OwnerPC, Function, Parms, OutParms, Stack, this);
    return true;
}

void URPG_TaskNodeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

#pragma endregion

#pragma region ActionTaskNode

void URPG_TaskNodeBase::ChangeStateTaskNode(ERPG_StateEntity NewState)
{
    if (TASK_NODE_CLOG(StateTaskNode == NewState, Warning, FString::Printf(TEXT("StateTaskNode equal New state: [%s]"), *UEnum::GetValueAsString(StateTaskNode)))) return;

    StateTaskNode = NewState;
    TASK_NODE_LOG(Display, FString::Printf(TEXT("StateTaskNode changed to: [%s]"), *UEnum::GetValueAsString(StateTaskNode)));
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::NotifyUpdateTaskNode);
    }
}

#pragma endregion
