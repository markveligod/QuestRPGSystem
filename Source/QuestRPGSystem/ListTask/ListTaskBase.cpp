// Fill out your copyright notice in the Description page of Project Settings.


#include "ListTask/ListTaskBase.h"
#include "Components/QuestManager.h"
#include "Librarys/QuestLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Task/TaskBase.h"

#pragma region LogListTask

void UListTaskBase::Print_LogListTask(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const
{
    if (!OwnerController || !ParentQuestManager) return;
    UQuestLibrary::Print_Log(LogVerb, FString::Printf(TEXT("NetMode: [%i] | OwnerController: [%s] | Parent Quest manager: [%s] | Name List task: [%s] | Type run ListTask: [%s] | %s"),
        OwnerController->GetNetMode(), *OwnerController->GetName(), *ParentQuestManager->GetName(), *GetName(), *UEnum::GetValueAsString(TypeRunListTask), *Text), Line, Function);
}

#pragma endregion

#pragma region Default

UListTaskBase::UListTaskBase()
{
    
}

bool UListTaskBase::InitListTask(APlayerController* PlayerController, UQuestManager* QuestManager)
{
    if (!CHECK_COND(PlayerController != nullptr, "Player controller is nullptr")) return false;
    if (!CHECK_COND(PlayerController->HasAuthority(), "Player controller is not authority")) return false;
    if (!CHECK_COND(QuestManager != nullptr, "Parent quest manager is nullptr")) return false;
    if (!CHECK_COND(StatusListTask == EStatusListTask::None, FString::Printf(TEXT("Status list task is: [%s] not equal None"),
        *UEnum::GetValueAsString(StatusListTask)))) return false;

    for (UTaskBase* Task : ArrayTask)
    {
        if (!CHECK_COND(Task != nullptr, "Task is nullptr")) return false;
        if (!CHECK_COND(Task->IsValidTask(), "Task is not valid")) return false;
    }
    
    OwnerController = PlayerController;
    ParentQuestManager = QuestManager;
    ChangeStateListTask(EStatusListTask::Init);
    return true;
}

bool UListTaskBase::RunListTask()
{
    if (!CHECK_COND(StatusListTask == EStatusListTask::Init, FString::Printf(TEXT("Status list task is: [%s] not equal Init"),
        *UEnum::GetValueAsString(StatusListTask)))) return false;

    switch (TypeRunListTask)
    {
        case ETypeRunListTask::StepByStep:
        {
            if (!CHECK_COND(ArrayTask.IsValidIndex(0), "Index 0 is not valid for array task")) return false;
            ProcessTasks(ArrayTask[0]);
            break;
        }
        case ETypeRunListTask::AllSameTime:
        {
            for (UTaskBase* Task : ArrayTask)
            {
                if (!CHECK_COND(Task != nullptr, "Task is nullptr")) return false;
                if (!CHECK_COND(Task->IsValidTask(), "Task is not valid")) return false;
                ProcessTasks(Task);
            }
            break;
        }
    }

    LOG_LIST_TASK(ELogVerb::Display, "Run list task");
    ChangeStateListTask(EStatusListTask::Run);
    return true;
}

bool UListTaskBase::CompleteListTask()
{
    LOG_LIST_TASK(ELogVerb::Display, "Complete list task");
    ChangeStateListTask(EStatusListTask::Complete);
    return true;
}

bool UListTaskBase::ResetListTask()
{
    LOG_LIST_TASK(ELogVerb::Display, "Reset list task");

    ResetAllTask();
    ChangeStateListTask(EStatusListTask::None);
    return true;
}

int32 UListTaskBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    return (OwnerController ? OwnerController->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool UListTaskBase::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
    LOG_LIST_TASK(ELogVerb::Display, FString::Printf(TEXT("CallRemoteFunction: %s"), *Function->GetName()));

    UNetDriver* NetDriver = OwnerController->GetNetDriver();
    if (!NetDriver)
    {
        LOG_LIST_TASK(ELogVerb::Error, "Net driver is nullptr");
        return false;
    }

    NetDriver->ProcessRemoteFunction(OwnerController, Function, Parms, OutParms, Stack, this);
    return true;
}

void UListTaskBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UListTaskBase, ArrayTask, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UListTaskBase, StatusListTask, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UListTaskBase, OwnerController, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UListTaskBase, ParentQuestManager, COND_OwnerOnly);
}

#pragma endregion

#pragma region ActionBase

FSoftObjectPath UListTaskBase::GetNextListTask() const
{
    return NextPathBlock;
}

void UListTaskBase::DestroyListTask()
{
    for (UTaskBase* Task : ArrayTask)
    {
        if (!Task) continue;
        Task->ResetTask();
        Task->MarkAsGarbage();
    }
    MarkAsGarbage();
}

void UListTaskBase::ProcessTasks(UTaskBase* Task)
{
    if (!CHECK_COND(Task != nullptr, "Task is nullptr")) return;

    const bool bStatusInitTask = Task->InitTask(OwnerController, this);
    Task->ChangeStatusTask(bStatusInitTask ? EStatusTask::Init : EStatusTask::Complete);

    switch (Task->GetStatusTask())
    {
        case EStatusTask::Init:
        {
            Task->OnUpdateTask.AddUObject(this, &ThisClass::RegisterUpdateTask);
            if (!Task->RunTask())
            {
                Task->ChangeStatusTask(EStatusTask::Complete);
                GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::NextInitTask);
            }
            break;
        }
        case EStatusTask::Complete:
        {
            GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::NextInitTask);
            break;
        }
    }

    OnUpdateListTask.Broadcast(this);
}

void UListTaskBase::NextInitTask()
{
    switch (TypeRunListTask)
    {
        case ETypeRunListTask::StepByStep:
        {
            const int32 Index = FindIndexLastNonCompleteTask();
            if (IsAllTaskComplete() || Index == INDEX_NONE)
            {
                CompleteListTask();
                return;
            }

            const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::ProcessTasks, ArrayTask[Index]);
            GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
            break;
        }
        case ETypeRunListTask::AllSameTime:
        {
            if (IsAllTaskComplete())
            {
                CompleteListTask();
                return;
            }
            break;
        }
    }
}

void UListTaskBase::RegisterUpdateTask(UTaskBase* Task)
{
    if (!CHECK_COND(Task != nullptr, "Task is nullptr")) return;

    if (Task->IsTaskComplete())
    {
        Task->OnUpdateTask.RemoveAll(this);
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::NextInitTask);
    }

    OnUpdateListTask.Broadcast(this);
}

int32 UListTaskBase::FindIndexLastNonCompleteTask() const
{
    for (int32 i = 0; i < ArrayTask.Num(); ++i)
    {
        if (!ArrayTask.IsValidIndex(i)) continue;
        const UTaskBase* Task = ArrayTask[i];
        if (!Task) continue;
        if (!Task->IsTaskComplete()) return i;
    }
    return INDEX_NONE;
}

int32 UListTaskBase::FindIndexFirstCompleteTask() const
{
    for (int32 i = 0; i < ArrayTask.Num(); ++i)
    {
        if (!ArrayTask.IsValidIndex(i)) continue;
        const UTaskBase* Task = ArrayTask[i];
        if (!Task) continue;
        if (Task->IsTaskComplete()) return i;
    }
    return INDEX_NONE;
}

bool UListTaskBase::IsAllTaskComplete() const
{
    for (const UTaskBase* Task : ArrayTask)
    {
        if (!Task) continue;
        if (!Task->IsTaskComplete()) return false;
    }
    return true;
}

bool UListTaskBase::IsSomeTaskComplete() const
{
    const auto FindElem = ArrayTask.FindByPredicate([](const UTaskBase* Task)
    {
        if (!Task) return false;
        return Task->IsTaskComplete();
    });
    return FindElem != nullptr;
}

void UListTaskBase::ChangeStateListTask(const EStatusListTask NewState)
{
    if (NewState == StatusListTask) return;

    StatusListTask = NewState;
    OnUpdateListTask.Broadcast(this);
}

void UListTaskBase::ResetAllTask()
{
    for (UTaskBase* Task : ArrayTask)
    {
        if (!Task) continue;
        Task->ResetTask();
    }
}

#pragma endregion
