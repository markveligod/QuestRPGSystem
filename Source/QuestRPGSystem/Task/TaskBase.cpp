// Fill out your copyright notice in the Description page of Project Settings.


#include "Task/TaskBase.h"
#include "ListTask/ListTaskBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

#pragma region LogTask

void UTaskBase::Print_LogTask(const TEnumAsByte<EQuestLogVerb> LogVerb, const FString Text, const int Line, const char* Function) const
{
    if (!OwnerController || !OwnerListTask) return;
    UQuestLibrary::Print_Log(LogVerb, FString::Printf(TEXT("NetMode: [%i] | OwnerController: [%s] | OwnerListTask: [%s] | Name: [%s] | %s"),
        OwnerController->GetNetMode(), *OwnerController->GetName(), *OwnerListTask->GetName(), *GetName(), *Text), Line, Function);
}

#pragma endregion

#pragma region Default

bool UTaskBase::InitTask(APlayerController* PlayerController, UListTaskBase* Parent)
{
    if (!CHECK_COND(PlayerController != nullptr, "Player controller is nullptr")) return false;
    if (!CHECK_COND(Parent != nullptr, "Parent list task is nullptr")) return false;

    OwnerController = PlayerController;
    OwnerListTask = Parent;

    LOG_TASK(Display, "init task base is success");
    ChangeStatusTask(EStatusTask::Init);
    return InitTask_Event(PlayerController, Parent);
}

bool UTaskBase::InitTask_Event_Implementation(APlayerController* PlayerController, UListTaskBase* Parent)
{
    return true;
}

bool UTaskBase::ResetTask()
{
    GetWorld()->GetTimerManager().ClearTimer(CompleteTaskTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(AbortTaskTimerHandle);

    LOG_TASK(Display, "reset task base is success");
    ChangeStatusTask(EStatusTask::NoneInit);
    return ResetTask_Event();
}

bool UTaskBase::ResetTask_Event_Implementation()
{
    return true;
}

bool UTaskBase::RunTask()
{
    if (TaskSpecificSettings.bEnableTimerTask)
    {
        RemainTimerCompleteTask = TaskSpecificSettings.TimerTask;
        GetWorld()->GetTimerManager().SetTimer(CompleteTaskTimerHandle, this, &ThisClass::CheckRemainTaskComplete, 1.0f, true);
    }

    if (TaskSpecificSettings.bEnableCheckAbortTask)
    {
        GetWorld()->GetTimerManager().SetTimer(AbortTaskTimerHandle, this, &ThisClass::CheckAbortTask, TaskSpecificSettings.CallFrequencyAbortTask, true);
    }
    
    LOG_TASK(Display, "run task base is success");
    ChangeStatusTask(EStatusTask::Run);
    return RunTask_Event();
}

bool UTaskBase::RunTask_Event_Implementation()
{
    return true;
}

bool UTaskBase::CompleteTask()
{
    GetWorld()->GetTimerManager().ClearTimer(CompleteTaskTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(AbortTaskTimerHandle);
    
    LOG_TASK(Display, "complete task base is success");
    ChangeStatusTask(EStatusTask::Complete);
    return CompleteTask_Event();
}

bool UTaskBase::CompleteTask_Event_Implementation()
{
    return true;
}

bool UTaskBase::IsValidTask()
{
    return IsValidTask_Event();
}

bool UTaskBase::IsValidTask_Event_Implementation()
{
    return true;
}

bool UTaskBase::IsAbortTask()
{
    return IsAbortTask_Event();
}

bool UTaskBase::IsAbortTask_Event_Implementation()
{
    return false;
}

int32 UTaskBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    return OwnerController ? OwnerController->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local;
}

bool UTaskBase::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
    LOG_TASK(Display, FString::Printf(TEXT("CallRemoteFunction: %s"), *Function->GetName()));

    if (!OwnerController && GetWorld()->GetNetMode() == NM_Client)
    {
        OwnerController = GetWorld()->GetFirstPlayerController();
    }
    if (!OwnerController) return false;
    UNetDriver* NetDriver = OwnerController->GetNetDriver();
    if (!NetDriver)
    {
        LOG_TASK(Error, "Net driver is nullptr");
        return false;
    }

    NetDriver->ProcessRemoteFunction(OwnerController, Function, Parms, OutParms, Stack, this);
    return true;
}

void UTaskBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UTaskBase, OwnerController, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UTaskBase, OwnerListTask, COND_OwnerOnly);
}

void UTaskBase::ServerCompleteTask_Implementation()
{
    CompleteTask();
}

#pragma endregion

#pragma region ActionBase

void UTaskBase::ChangeStatusTask(const EStatusTask& NewStatus)
{
    if (StatusTask == NewStatus)
    {
        LOG_TASK(Warning, FString::Printf(TEXT("Current status task equal new status: [%s]"),
            *UEnum::GetValueAsString(NewStatus)));
        return;
    }

    LOG_TASK(Display, FString::Printf(TEXT("New status task: [%s]"), *UEnum::GetValueAsString(NewStatus)));
    StatusTask = NewStatus;
    OnUpdateTask.Broadcast(this);
}

TArray<FDrawDebugQuestData> UTaskBase::GetDrawDebugData()
{
    return GetDrawDebugData_Event();
}

TArray<FDrawDebugQuestData> UTaskBase::GetDrawDebugData_Event_Implementation()
{
    TArray<FDrawDebugQuestData> Result;

    return Result;
}

void UTaskBase::CheckAbortTask()
{
    if (IsAbortTask())
    {
        GetWorld()->GetTimerManager().ClearTimer(AbortTaskTimerHandle);
        CompleteTask();
    }
}

void UTaskBase::CheckRemainTaskComplete()
{
    RemainTimerCompleteTask -= 1.0f;
    TimerDescription = UQuestLibrary::GetStringTimeFromSecond(RemainTimerCompleteTask);
    OnUpdateTask.Broadcast(this);
    if (RemainTimerCompleteTask <= 0.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(CompleteTaskTimerHandle);

        // TODO: Вызвать досрочное завершение квеста
    }
}

#pragma endregion
