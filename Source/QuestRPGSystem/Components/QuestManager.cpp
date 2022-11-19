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
    DataQuest.ArrayDataListTask.Add(FDataListTask(QuestFromTable->StartListTask));
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
    if (!UQuestLibrary::CheckIsClient(GetOwner()) && DataQuest.ActiveListTask)
    {
        PushReplicateID(QuestName);
    }
}

bool UQuestManager::ServerChangeTargetQuest_Validate(const FName QuestName)
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

    if (DataQuest.ActiveListTask)
    {
        DataQuest.ActiveListTask->DestroyListTask();
        DataQuest.ActiveListTask = nullptr;
    }

    // TODO: имплементировать статус завершения квеста
    // DataQuest.StatusQuest = QuestFromTable->TypeQuest == ETypeQuest::Event ? EStatusQuest::CompleteFail : EStatusQuest::CompleteSuccess;
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

    FDataListTask& DataListTask = DataQuest.ArrayDataListTask[0];
    UListTaskBase* StartVisibleBlock = Cast<UListTaskBase>(UQuestLibrary::CreateListTaskFromPath(GetOwner(), DataListTask.PathToListTask));
    if (!CHECK_COND(StartVisibleBlock != nullptr, "Start visible block is fail load in memory")) return;
    if (!CHECK_COND(StartVisibleBlock->GetTypeListTask() == ETypeListTask::Visible, "Start visible block is not visible")) return;

    const bool bStatusInitListTask = StartVisibleBlock->InitListTask(Cast<APlayerController>(GetOwner()), this);
    if (!CHECK_COND(bStatusInitListTask, FString::Printf(TEXT("List task: [%s] is none init"),
        *StartVisibleBlock->GetName())))
        return;

    DataQuest.ActiveListTask = StartVisibleBlock;
    DataListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(StartVisibleBlock);
    DataQuest.ActiveListTask->OnUpdateListTask.AddUObject(this, &ThisClass::RegisterUpdateListTask);

    const bool bStatusRunListTask = DataQuest.ActiveListTask->RunListTask();
    if (!CHECK_COND(bStatusRunListTask, FString::Printf(TEXT("List task: [%s] is none run"), *DataQuest.ActiveListTask->GetName()))) return;

    DataQuest.StatusQuest = EStatusQuest::InProcess;
    NotifyUpdateQuest(QuestName);

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(this, &ThisClass::ServerChangeTargetQuest, QuestName);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
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
    if (DataQuest.ActiveListTask == nullptr)
    {
        CompleteQuest(QuestName);
        return;
    }

    const FSoftObjectPath NextPath = DataQuest.ActiveListTask->GetNextListTask();
    if (NextPath == FSoftObjectPath())
    {
        CompleteQuest(QuestName);
        return;
    }
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

void UQuestManager::InitNextVisibleListTask(const FName QuestName, const FSoftObjectPath NextVisibleLTPath)
{
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;
    if (!CHECK_COND(NextVisibleLTPath.IsValid(), "Next visible list task is none")) return;
    if (!CHECK_COND(DataQuest.ActiveListTask != nullptr, "Active visible list task is nullptr")) return;

    // TODO: отрефакторить эту часть
    // FDataListTask& DataListTask = GetDataVisibleListFromListTask(QuestName, DataQuest.ActiveVisibleListTask);
    // DataListTask.bListTaskComplete = true;

    DataQuest.ActiveListTask->OnUpdateListTask.RemoveAll(this);
    DataQuest.ActiveListTask->DestroyListTask();
    DataQuest.ActiveListTask = nullptr;

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

    FDataListTask NewDataVisibleListTask(NextVisibleLTPath);
    NewDataVisibleListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(NextVisibleBlock);
    NextVisibleBlock->OnUpdateListTask.AddUObject(this, &ThisClass::RegisterUpdateListTask);
    DataQuest.ArrayDataListTask.Add(NewDataVisibleListTask);
    DataQuest.ActiveListTask = NextVisibleBlock;

    NotifyUpdateQuest(QuestName);
}

#pragma endregion
