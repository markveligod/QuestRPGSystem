// Fill out your copyright notice in the Description page of Project Settings.


#include "Librarys/QuestLibrary.h"

#include "Engine/AssetManager.h"
#include "Kismet/KismetStringLibrary.h"
#include "ListTask/ListTaskBase.h"
#include "Task/TaskBase.h"

void UQuestLibrary::Print_Log(const TEnumAsByte<EQuestLogVerb> LogVerb, const FString Text, const int Line, const char* Function)
{
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

bool UQuestLibrary::CheckedCondition(const bool bCondition, const FString Text, const int Line, const char* Function)
{
    if (!bCondition)
    {
        Print_Log(Error, Text, Line, Function);
        return false;
    }
    return true;
}

FString UQuestLibrary::GetStringTimeFromSecond(float Seconds)
{
    FString Time = UKismetStringLibrary::TimeSecondsToString(Seconds);
    Time = Time.Mid(0, 5);
    return Time;
}

UListTaskBase* UQuestLibrary::LoadListTaskFromPath(UObject* Owner, const FSoftObjectPath& PathListTask)
{
    if (!CHECK_COND(PathListTask.IsValid(), "Path list task is none")) return nullptr;
    const UClass* CDO = LoadClass<UListTaskBase>(Owner, *(PathListTask.ToString()));
    if (!CHECK_COND(CDO != nullptr, "CDO is nullptr")) return nullptr;
    return NewObject<UListTaskBase>(Owner, CDO);
}

void UQuestLibrary::UnLoadListTaskFromPath(const FSoftObjectPath& PathListTask)
{
    FStreamableManager Streamable;
    Streamable.Unload(PathListTask);
}

TArray<FDataInfoTask> UQuestLibrary::FillDataInfoTasksFromListTask(UListTaskBase* ListTask)
{
    TArray<FDataInfoTask> ResultDataInfoTask;
    if (!CHECK_COND(ListTask != nullptr, "List task is nullptr")) return ResultDataInfoTask;

    TArray<UTaskBase*> ArrayTasks = ListTask->GetArrayTask();
    for (int32 i = 0; i < ArrayTasks.Num(); ++i)
    {
        if (!ArrayTasks.IsValidIndex(i)) continue;
        const UTaskBase* Task = ArrayTasks[i];
        FDataInfoTask TempDataInfoTask;
        TempDataInfoTask.TaskDescription = Task->GetTaskDescription();
        TempDataInfoTask.bStatusTask = Task->GetStatusTask() == EStatusTask::Complete;
        TempDataInfoTask.TaskID = FString::Printf(TEXT("Index - %i - List Task Path - %s - Task Class - %s"),
            i, *ListTask->GetClass()->GetName(), *Task->GetClass()->GetName());
        ResultDataInfoTask.Add(TempDataInfoTask);
    }

    return ResultDataInfoTask;
}

bool UQuestLibrary::CheckIsClient(const AActor* InActor)
{
    if (InActor == nullptr) return false;

    const ENetMode& NetMode = InActor->GetNetMode();

    if (NetMode == NM_Standalone)
    {
        // Not networked.
        return true;
    }

    if (NetMode == NM_DedicatedServer)
    {
        // Pure server.
        return false;
    }

    const ENetRole& LR = InActor->GetLocalRole();
    const ENetRole& RR = InActor->GetRemoteRole();

    if (NetMode == NM_Client)
    {
        // Or Networked client in control.
        return LR == ROLE_AutonomousProxy;
    }

    if (NetMode == NM_ListenServer)
    {
        // Networked client in control.
        return RR == ROLE_SimulatedProxy;
    }

    return false;
}

FString UQuestLibrary::GetNetModeToString(const AActor* Actor)
{
    if (!Actor) return "";
    
    switch (Actor->GetNetMode())
    {
        case NM_Client:
        {
            return "NM_Client";
        }
        case NM_DedicatedServer:
        {
            return "NM_DedicatedServer";
        }
        case NM_Standalone:
        {
            return "NM_Standalone";
        }
        case NM_ListenServer:
        {
            return "NM_ListenServer";
        }
    }
    return "";
}
