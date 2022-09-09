// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestManager.h"
#include "Librarys/QuestLibrary.h"
#include "ListTask/ListTaskBase.h"

#pragma region API_Checked

EStatusQuest UQuestManager::GetStatusQuest(const FName& QuestName)
{
    const auto FindElem = ArrayDataQuest.FindByPredicate([QuestName](const FDataQuest& DataQuest)
    {
        return DataQuest.NameQuestTable == QuestName;
    });

    return FindElem ? FindElem->StatusQuest : EStatusQuest::HaveNot;
}

#pragma endregion

#pragma region API_Action

void UQuestManager::ServerAddQuest_Implementation(const FName& QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    if (!CHECK_COND(GetStatusQuest(QuestName) == EStatusQuest::HaveNot, "Quest exist in array")) return;
    if (!CHECK_COND(DataQuestTable != nullptr, "Data quest table is nullptr")) return;

    FDataQuestTable* QuestFromTable = DataQuestTable->FindRow<FDataQuestTable>(QuestName, "");
    if (!CHECK_COND(QuestFromTable != nullptr, "Data quest from table is not found")) return;

    FDataQuest DataQuest(QuestName);
    DataQuest.ArrayDataListTask.Add(FDataVisibleListTask(QuestFromTable->StartVisibleListTask));
    for (const auto HiddenLTPath : QuestFromTable->ArrayHiddenListTask)
    {
        DataQuest.ArrayHiddenListTasks.Add(FDataHiddenListTask(HiddenLTPath));
    }
    ArrayDataQuest.AddUnique(DataQuest);

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(this, &ThisClass::StartInitQuest, QuestName);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
}

bool UQuestManager::ServerAddQuest_Validate(const FName& QuestName)
{
    // TODO: Validate
    return true;
}


void UQuestManager::ServerAddArrayQuest_Implementation(const TArray<FName>& ArrayQuestName)
{
    for (const FName& NameQuest : ArrayQuestName)
    {
        ServerAddQuest(NameQuest);
    }
}

bool UQuestManager::ServerAddArrayQuest_Validate(const TArray<FName>& ArrayQuestName)
{
    // TODO: Validate
    return true;
}

void UQuestManager::ServerChangeTargetQuest_Implementation(const FName QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    for (auto& Data : ArrayDataQuest)
    {
        Data.bIsTargetQuest = false;
    }

    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    DataQuest.bIsTargetQuest = true;
    OnSwitchQuest.Broadcast(DataQuest.NameQuestTable);
    if (!UQuestLibrary::CheckIsClient(GetOwner()) && DataQuest.ActiveVisibleListTask)
    {
        PushReplicateID(QuestName);
    }
}

bool UQuestManager::ServerChangeTargetQuest_Validate(const FName QuestName)
{
    // TODO: Validate
    return true;
}

void UQuestManager::ServerAddHiddenListTask_Implementation(const FName& QuestName, const FSoftClassPath& HiddenListTaskPath)
{
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;
    if (!CHECK_COND(HiddenListTaskPath.IsValid(), "HiddenListTaskPath is none")) return;

    DataQuest.ArrayHiddenListTasks.Add(FDataHiddenListTask(HiddenListTaskPath));
    CheckedInitHiddenListTask(QuestName);
    CheckedRunHiddenListTask(QuestName);
}

bool UQuestManager::ServerAddHiddenListTask_Validate(const FName& QuestName, const FSoftClassPath& HiddenListTaskPath)
{
    // TODO: Validate
    return true;
}

void UQuestManager::CompleteQuest(const FName QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;

    FDataQuestTable* QuestFromTable = DataQuestTable->FindRow<FDataQuestTable>(QuestName, "");
    if (!CHECK_COND(QuestFromTable != nullptr, "Data quest from table is not found")) return;

    for (auto& DataVisibleLT : DataQuest.ArrayDataListTask)
    {
        DataVisibleLT.bListTaskComplete = true;
    }

    if (DataQuest.ActiveVisibleListTask)
    {
        DataQuest.ActiveVisibleListTask->DestroyListTask();
        DataQuest.ActiveVisibleListTask = nullptr;
    }

    for (auto& DataHiddenLT : DataQuest.ArrayHiddenListTasks)
    {
        if (DataHiddenLT.ActiveHiddenListTask)
        {
            DataHiddenLT.bListTaskComplete = true;
            DataHiddenLT.ActiveHiddenListTask->DestroyListTask();
            DataHiddenLT.ActiveHiddenListTask = nullptr;
        }
    }

    DataQuest.StatusQuest = QuestFromTable->TypeQuest == ETypeQuest::Event ? EStatusQuest::CompleteFail : EStatusQuest::CompleteSuccess;
    ClientSendNotifyCompleteQuest(QuestName);
}

void UQuestManager::StartInitQuest(const FName QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    if (!CHECK_COND(GetOwner()->HasAuthority(), "Owner is not Authority")) return;

    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;
    if (!CHECK_COND(DataQuest.ArrayDataListTask.Num() == 1, "This is a broken set with more than 1 visible list task")) return;

    DataQuest.StatusQuest = EStatusQuest::Init;
    ClientSendNotifyStartQuest(DataQuest.NameQuestTable);

    StartInitListTask(DataQuest.NameQuestTable);
}

void UQuestManager::StartInitListTask(const FName& QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    if (!CHECK_COND(GetOwner()->HasAuthority(), "Owner is not Authority")) return;
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;

    FDataVisibleListTask& DataVisibleListTask = DataQuest.ArrayDataListTask[0];
    UListTaskBase* StartVisibleBlock = Cast<UListTaskBase>(UQuestLibrary::CreateListTaskFromPath(GetOwner(), DataVisibleListTask.PathToVisibleListTask));
    if (!CHECK_COND(StartVisibleBlock != nullptr, "Start visible block is fail load in memory")) return;
    if (!CHECK_COND(StartVisibleBlock->GetTypeListTask() == ETypeListTask::Visible, "Start visible block is not visible")) return;

    const bool bStatusInitListTask = StartVisibleBlock->InitListTask(Cast<APlayerController>(GetOwner()), this);
    if (!CHECK_COND(bStatusInitListTask, FString::Printf(TEXT("List task: [%s] is none init"),
        *StartVisibleBlock->GetName())))
        return;

    DataQuest.ActiveVisibleListTask = StartVisibleBlock;
    DataVisibleListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(StartVisibleBlock);
    DataQuest.ActiveVisibleListTask->OnUpdateListTask.AddUObject(this, &ThisClass::RegisterUpdateListTask);

    if (DataQuest.ActiveVisibleListTask->GetPathActionWorld().GetAssetName() != GetWorld()->GetName())
    {
        RejectQuest(QuestName);
    }
    else
    {
        const bool bStatusRunListTask = DataQuest.ActiveVisibleListTask->RunListTask();
        if (!CHECK_COND(bStatusRunListTask, FString::Printf(TEXT("List task: [%s] is none run"),
            *DataQuest.ActiveVisibleListTask->GetName())))
            return;
    }
    DataQuest.StatusQuest = EStatusQuest::InProcess;
    NotifyUpdateQuest(QuestName);

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(this, &ThisClass::ServerChangeTargetQuest, QuestName);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);

    CheckedInitHiddenListTask(QuestName);
    CheckedRunHiddenListTask(QuestName);
}

void UQuestManager::RegisterUpdateListTask(UListTaskBase* ListTask)
{
    if (!CHECK_COND(ListTask != nullptr, "List task is nullptr")) return;
    const FDataQuest& DataQuest = FindDataQuestFromID(ListTask->GetUniqueID());

    UpdateInfoDataQuest(DataQuest.NameQuestTable);
    NotifyUpdateQuest(DataQuest.NameQuestTable);

    if (ListTask->GetStatusListTask() == EStatusListTask::Complete)
    {
        ListTask->OnUpdateListTask.RemoveAll(this);
        FTimerHandle TimerHandle;
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUObject(this, &ThisClass::NextListTask, DataQuest.NameQuestTable);
        GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
    }
}

void UQuestManager::NextListTask(const FName QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    if (!CHECK_COND(!QuestName.IsNone(), "Quest name is none")) return;
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (DataQuest.ActiveVisibleListTask == nullptr)
    {
        CompleteQuest(QuestName);
        return;
    }

    const FSoftObjectPath NextPath = DataQuest.ActiveVisibleListTask->GetNextListTask();
    InitNextVisibleListTask(QuestName, NextPath);
}

void UQuestManager::NotifyUpdateQuest(const FName& QuestName)
{
    if (!UQuestLibrary::CheckIsClient(GetOwner()))
    {
        PushReplicateID(QuestName);
    }
    ClientSendNotifyUpdateQuest(QuestName);
}

void UQuestManager::RejectQuest(const FName& QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
}

void UQuestManager::InitNextVisibleListTask(const FName QuestName, const FSoftObjectPath NextVisibleLTPath)
{
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;
    if (!CHECK_COND(NextVisibleLTPath.IsValid(), "Next visible list task is none")) return;
    if (!CHECK_COND(DataQuest.ActiveVisibleListTask != nullptr, "Active visible list task is nullptr")) return;

    FDataVisibleListTask& DataVisibleListTask = GetDataVisibleListFromListTask(QuestName, DataQuest.ActiveVisibleListTask);
    DataVisibleListTask.bListTaskComplete = true;

    DataQuest.ActiveVisibleListTask->OnUpdateListTask.RemoveAll(this);
    DataQuest.ActiveVisibleListTask->DestroyListTask();
    DataQuest.ActiveVisibleListTask = nullptr;

    UListTaskBase* NextVisibleBlock = Cast<UListTaskBase>(UQuestLibrary::CreateListTaskFromPath(GetOwner(), NextVisibleLTPath));
    if (!NextVisibleBlock)
    {
        CompleteQuest(QuestName);
        return;
    }

    if (NextVisibleBlock->GetTypeListTask() != ETypeListTask::Visible)
    {
        CompleteQuest(QuestName);
        return;
    }

    const bool bStatusInitListTask = NextVisibleBlock->InitListTask(Cast<APlayerController>(GetOwner()), this);
    if (!bStatusInitListTask)
    {
        CompleteQuest(QuestName);
        return;
    }

    const bool bStatusRunListTask = NextVisibleBlock->RunListTask();
    if (!bStatusRunListTask)
    {
        CompleteQuest(QuestName);
        return;
    }

    FDataVisibleListTask NewDataVisibleListTask(NextVisibleLTPath);
    NewDataVisibleListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(NextVisibleBlock);
    NextVisibleBlock->OnUpdateListTask.AddUObject(this, &ThisClass::RegisterUpdateListTask);
    DataQuest.ArrayDataListTask.Add(NewDataVisibleListTask);
    DataQuest.ActiveVisibleListTask = NextVisibleBlock;

    NotifyUpdateQuest(QuestName);

    CheckedInitHiddenListTask(QuestName);
    CheckedRunHiddenListTask(QuestName);
}

#pragma endregion

#pragma region HiddenListTaskProcess

void UQuestManager::CheckedInitHiddenListTask(const FName& QuestName)
{
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;

    TArray<FDataHiddenListTask>& ArrayHiddenListTask = DataQuest.ArrayHiddenListTasks;
    for (auto& HiddenLT : ArrayHiddenListTask)
    {
        if (HiddenLT.ActiveHiddenListTask || HiddenLT.bListTaskComplete) continue;
        HiddenLT.ActiveHiddenListTask = Cast<UListTaskBase>(UQuestLibrary::CreateListTaskFromPath(GetOwner(), HiddenLT.PathToHiddenListTask));
        if (!HiddenLT.ActiveHiddenListTask) continue;
        HiddenLT.ActiveHiddenListTask->InitListTask(Cast<APlayerController>(GetOwner()), this);
    }
}

void UQuestManager::CheckedRunHiddenListTask(const FName& QuestName)
{
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;

    TArray<FDataHiddenListTask>& ArrayHiddenListTask = DataQuest.ArrayHiddenListTasks;
    for (auto& HiddenLT : ArrayHiddenListTask)
    {
        if (HiddenLT.bListTaskComplete) continue;
        auto* ActiveLT = HiddenLT.ActiveHiddenListTask;
        if (!ActiveLT) continue;
        if (ActiveLT->GetTypeListTask() != ETypeListTask::Hidden) continue;
        if (ActiveLT->GetStatusListTask() != EStatusListTask::Init) continue;
        switch (ActiveLT->GetRunHiddenListTask())
        {
            case ERunHiddenListTask::AddToQuest:
            {
                ActiveLT->OnUpdateListTask.AddUObject(this, &ThisClass::RegisterUpdateHiddenListTask);
                ActiveLT->RunListTask();
                break;
            }
            case ERunHiddenListTask::InitToRunListTask:
            {
                if (!DataQuest.ActiveVisibleListTask) continue;
                if (ActiveLT->GetToInitVisibleBlock().GetAssetPathString() == DataQuest.ActiveVisibleListTask->GetClass()->GetPathName())
                {
                    ActiveLT->OnUpdateListTask.AddUObject(this, &ThisClass::RegisterUpdateHiddenListTask);
                    HiddenLT.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(ActiveLT);
                    ActiveLT->RunListTask();
                }
                break;
            }
        }
    }
}

void UQuestManager::RegisterUpdateHiddenListTask(UListTaskBase* HiddenLT)
{
    if (!CHECK_COND(HiddenLT != nullptr, "Hidden List task is nullptr")) return;
    const FDataQuest& DataQuest = GetDataQuestFromHiddenLT(HiddenLT);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;

    if (HiddenLT->GetStatusListTask() == EStatusListTask::Complete)
    {
        HiddenLT->OnUpdateListTask.RemoveAll(this);
        switch (HiddenLT->GetActionHiddenListTask())
        {
            case EActionHiddenListTask::CompleteQuest:
            {
                FTimerHandle TimerHandle;
                FTimerDelegate TimerDelegate;
                TimerDelegate.BindUObject(this, &ThisClass::CompleteQuest, DataQuest.NameQuestTable);
                GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
                break;
            }
            case EActionHiddenListTask::ForceTransferToNextBlock:
            {
                FTimerHandle TimerHandle;
                FTimerDelegate TimerDelegate;
                TimerDelegate.BindUObject(this, &ThisClass::InitNextVisibleListTask, DataQuest.NameQuestTable, HiddenLT->GetToInitVisibleBlock());
                GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
                break;
            }
        }

        FDataHiddenListTask& DataHiddenListTask = GetDataHiddenListFromListTask(DataQuest.NameQuestTable, HiddenLT);
        DataHiddenListTask.bListTaskComplete = true;
        DataHiddenListTask.ActiveHiddenListTask->DestroyListTask();
        DataHiddenListTask.ActiveHiddenListTask = nullptr;
    }
}

#pragma endregion
