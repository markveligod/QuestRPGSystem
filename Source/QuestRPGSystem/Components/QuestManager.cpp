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
    DataQuest.StatusQuest = EStatusQuest::Init;
    
    DataQuest.ArrayDataListTask.Add(FDataListTask(QuestFromTable->StartListTask));
    ArrayDataQuest.AddUnique(DataQuest);

    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(this, &ThisClass::StartInitQuest, QuestName);
    GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
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

void UQuestManager::CompleteQuest(const FName QuestName, const bool bSuccessComplete)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;

    const FDataQuestTable* QuestFromTable = DataQuestTable->FindRow<FDataQuestTable>(QuestName, "");
    if (!CHECK_COND(QuestFromTable != nullptr, "Data quest from table is not found")) return;

    for (auto& DataVisibleLT : DataQuest.ArrayDataListTask)
    {
        DataVisibleLT.bListTaskComplete = true;
    }

    DestroyActiveListTaskFromQuestName(QuestName);
    DataQuest.StatusQuest = bSuccessComplete ? EStatusQuest::CompleteSuccess : EStatusQuest::CompleteFail;
    ClientSendNotifyCompleteQuest(QuestName);
}

void UQuestManager::StartInitQuest(const FName QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    if (!CHECK_COND(GetOwner()->HasAuthority(), "Owner is not Authority")) return;

    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;
    if (!CHECK_COND(DataQuest.ArrayDataListTask.Num() == 1, "This is a broken set with more than 1 visible list task")) return;

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
    UListTaskBase* StartBlock = Cast<UListTaskBase>(UQuestLibrary::CreateListTaskFromPath(GetOwner(), DataListTask.PathToListTask));
    if (!CHECK_COND(StartBlock != nullptr, "Start visible block is fail load in memory")) return;

    const bool bStatusInitListTask = StartBlock->InitListTask(Cast<APlayerController>(GetOwner()), this);
    if (!CHECK_COND(bStatusInitListTask, FString::Printf(TEXT("List task: [%s] is none init"),
        *StartBlock->GetName())))
        return;

    DataQuest.ActiveListTask = StartBlock;
    DataListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(StartBlock);
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
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUObject(this, &ThisClass::NextListTask, DataQuest.NameQuestTable);
        GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
    }
}

void UQuestManager::NextListTask(const FName QuestName)
{
    LOG_QM(ELogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    if (!CHECK_COND(!QuestName.IsNone(), "Quest name is none")) return;
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(!DataQuest.IsEmpty(), "Data quest is empty")) return;

    if (DataQuest.ActiveListTask == nullptr)
    {
        CompleteQuest(QuestName, false);
        return;
    }

    const FSoftObjectPath NextPath = DataQuest.ActiveListTask->GetNextListTask();
    if (!NextPath.IsValid())
    {
        CompleteQuest(QuestName, false);
        return;
    }
    
    FDataListTask& DataListTask = GetDataListTaskFromPathListTask(DataQuest.ActiveListTask);
    DataListTask.bListTaskComplete = true;
    DestroyActiveListTaskFromQuestName(QuestName);
    
    InitListTask(DataQuest, NextPath);
}

void UQuestManager::NotifyUpdateQuest(const FName& QuestName)
{
    if (!UQuestLibrary::CheckIsClient(GetOwner()))
    {
        PushReplicateID(QuestName);
    }
    ClientSendNotifyUpdateQuest(QuestName);
}

void UQuestManager::InitListTask(FDataQuest& DataQuest, const FSoftObjectPath& NextLTPath)
{
    UListTaskBase* NextVisibleBlock = Cast<UListTaskBase>(UQuestLibrary::CreateListTaskFromPath(GetOwner(), NextLTPath));
    if (!NextVisibleBlock)
    {
        CompleteQuest(DataQuest.NameQuestTable, false);
        return;
    }

    const bool bStatusInitListTask = NextVisibleBlock->InitListTask(Cast<APlayerController>(GetOwner()), this);
    if (!bStatusInitListTask)
    {
        CompleteQuest(DataQuest.NameQuestTable, false);
        return;
    }

    const bool bStatusRunListTask = NextVisibleBlock->RunListTask();
    if (!bStatusRunListTask)
    {
        CompleteQuest(DataQuest.NameQuestTable, false);
        return;
    }

    FDataListTask NewDataVisibleListTask(NextLTPath);
    NewDataVisibleListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(NextVisibleBlock);
    NextVisibleBlock->OnUpdateListTask.AddUObject(this, &ThisClass::RegisterUpdateListTask);
    DataQuest.ArrayDataListTask.Add(NewDataVisibleListTask);
    DataQuest.ActiveListTask = NextVisibleBlock;

    NotifyUpdateQuest(DataQuest.NameQuestTable);
}

#pragma endregion
