// Fill out your copyright notice in the Description page of Project Settings.


#include "Task/TaskBase.h"
#include "ListTask/ListTaskBase.h"
#include "Kismet/KismetSystemLibrary.h"

#pragma region LogTask

void UTaskBase::Print_LogTask(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const
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

    LOG_TASK(ELogVerb::Display, "init task base is success");
    ChangeStatusTask(EStatusTask::Init);
    return true;
}

bool UTaskBase::ResetTask()
{
    LOG_TASK(ELogVerb::Display, "reset task base is success");
    ChangeStatusTask(EStatusTask::NoneInit);
    return true;
}

bool UTaskBase::RunTask()
{
    LOG_TASK(ELogVerb::Display, "run task base is success");
    ChangeStatusTask(EStatusTask::Run);
    return true;
}

bool UTaskBase::CompleteTask()
{
    LOG_TASK(ELogVerb::Display, "complete task base is success");
    ChangeStatusTask(EStatusTask::Complete);
    return true;
}

bool UTaskBase::IsValidTask()
{
    return true;
}

#pragma endregion

#pragma region ActionBase

void UTaskBase::ChangeStatusTask(const EStatusTask& NewStatus)
{
    if (StatusTask == NewStatus)
    {
        LOG_TASK(ELogVerb::Warning, FString::Printf(TEXT("Current status task equal new status: [%s]"),
            *UEnum::GetValueAsString(NewStatus)));
        return;
    }

    LOG_TASK(ELogVerb::Display, FString::Printf(TEXT("New status task: [%s]"), *UEnum::GetValueAsString(NewStatus)));
    StatusTask = NewStatus;
    OnUpdateTask.Broadcast(this);
}

TArray<FTaskPoint> UTaskBase::GenerateTaskPointForMissMap()
{
    TArray<FTaskPoint> Result;

    for (int32 i = 0; i < TaskSpecificSettings.ArrPointPositionForMap.Num(); ++i)
    {
        const FVector& Pos = TaskSpecificSettings.ArrPointPositionForMap[i];
        FTaskPoint TaskPoint;
        TaskPoint.AlternatePos = Pos;
        TaskPoint.bStaticActor = true;
        TaskPoint.MarkDeviation = TaskSpecificSettings.TargetMarkDeviation;
        TaskPoint.Tag = GetName() + FString::FromInt(i);
        TaskPoint.Description = TaskDescription;
        TaskPoint.IsDetective = TaskSpecificSettings.bEnableCompassSearch;
        TaskPoint.RadiusSearch = TaskSpecificSettings.RadiusSearch;
        TaskPoint.bEnableZone = TaskSpecificSettings.bEnableZone;
        TaskPoint.CenterZone = TaskSpecificSettings.CenterZone;
        TaskPoint.RadiusZone = TaskSpecificSettings.RadiusZone;
    }
    return Result;
}

void UTaskBase::ClientDrawPoint_Implementation(const FVector& Position)
{
    if (OwnerController)
    {
        DrawDebugSphere(OwnerController->GetWorld(), Position, 50.0f, 12, FColor::Yellow, false, 1.0f, 0, 2.0f);
    }
}

#pragma endregion
