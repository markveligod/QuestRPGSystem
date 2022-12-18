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
    AddQuest(QuestName);
}

bool UQuestManager::ServerAddQuest_Validate(const FName& QuestName)
{
    return CheckedAddQuest(QuestName);
}


void UQuestManager::ServerAddArrayQuest_Implementation(const TArray<FName>& ArrayQuestName)
{
    for (const FName& NameQuest : ArrayQuestName)
    {
        AddQuest(NameQuest);
    }
}

bool UQuestManager::ServerAddArrayQuest_Validate(const TArray<FName>& ArrayQuestName)
{
    return CheckedAddArrayQuest(ArrayQuestName);
}

void UQuestManager::ServerInitListTask_Implementation(const FName& QuestName, const FSoftClassPath& ListTaskPath)
{
    InitListTask(QuestName, FSoftObjectPath(ListTaskPath));
}

bool UQuestManager::ServerInitListTask_Validate(const FName& QuestName, const FSoftClassPath& ListTaskPath)
{
    return CheckedInitListTask(QuestName, ListTaskPath);
}

bool UQuestManager::CheckedAddQuest(const FName& QuestName)
{
    return true;
}

bool UQuestManager::CheckedAddArrayQuest(const TArray<FName>& ArrayQuestName)
{
    return true;
}

bool UQuestManager::CheckedInitListTask(const FName& QuestName, const FSoftClassPath& ListTaskPath)
{
    return true;
}

void UQuestManager::AddQuest(const FName& QuestName)
{
    LOG_QM(EQuestLogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    if (!CHECK_COND(GetStatusQuest(QuestName) == EStatusQuest::HaveNot, "Quest exist in array")) return;
    if (!CHECK_COND(DataQuestTable != nullptr, "Data quest table is nullptr")) return;

    const FDataQuestTable* QuestFromTable = DataQuestTable->FindRow<FDataQuestTable>(QuestName, "");
    if (!CHECK_COND(QuestFromTable != nullptr, "Data quest from table is not found")) return;

    FDataQuest DataQuest(QuestName);
    DataQuest.StatusQuest = EStatusQuest::Start;
    
    DataQuest.ArrayDataListTask.Add(FDataListTask(QuestFromTable->StartListTask));
    ArrayDataQuest.AddUnique(DataQuest);

    ClientSendNotifyStartQuest(DataQuest.NameQuestTable);

    const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::InitListTask, QuestName, FSoftObjectPath(QuestFromTable->StartListTask));
    GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}

void UQuestManager::CompleteQuest(const FName QuestName)
{
    LOG_QM(EQuestLogVerb::Display, FString::Printf(TEXT("QuestName: [%s]"), *QuestName.ToString()));

    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;

    const FDataQuestTable* QuestFromTable = DataQuestTable->FindRow<FDataQuestTable>(QuestName, "");
    if (!CHECK_COND(QuestFromTable != nullptr, "Data quest from table is not found")) return;

    for (auto& DataVisibleLT : DataQuest.ArrayDataListTask)
    {
        DataVisibleLT.bListTaskComplete = true;
    }

    DestroyActiveListTaskFromQuestName(QuestName);
    DataQuest.StatusQuest = EStatusQuest::Complete;
    ClientSendNotifyCompleteQuest(QuestName);
}

void UQuestManager::InitListTask(const FName QuestName, const FSoftObjectPath ListTask)
{
    if (!CHECK_COND(GetOwner()->HasAuthority(), "Owner is not Authority")) return;

    LOG_QM(EQuestLogVerb::Display, FString::Printf(TEXT("QuestName: [%s] | ListTask: [%s]"), *QuestName.ToString(),
        *ListTask.ToString()));

    DestroyActiveListTaskFromQuestName(QuestName);

    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;

    DataQuest.StatusQuest = EStatusQuest::InProcess;
    UListTaskBase* NextVisibleBlock = Cast<UListTaskBase>(UQuestLibrary::LoadListTaskFromPath(GetOwner(), ListTask));
    if (!NextVisibleBlock)
    {
        CompleteQuest(QuestName);
        return;
    }

    if (!NextVisibleBlock->InitListTask(Cast<APlayerController>(GetOwner()), this))
    {
        CompleteQuest(QuestName);
        return;
    }

    if (!NextVisibleBlock->RunListTask())
    {
        CompleteQuest(QuestName);
        return;
    }

    FDataListTask NewDataVisibleListTask(ListTask);
    NewDataVisibleListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(NextVisibleBlock);
    NextVisibleBlock->OnUpdateListTask.AddUObject(this, &ThisClass::RegisterUpdateListTask);
    DataQuest.ArrayDataListTask.Add(NewDataVisibleListTask);
    DataQuest.ActiveListTask = NextVisibleBlock;

    NotifyUpdateQuest(DataQuest.NameQuestTable);
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
        FDataListTask& DataListTask = GetDataListTaskFromPathListTask(DataQuest.ActiveListTask);
        DataListTask.bListTaskComplete = true;

        const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::InitListTask, DataQuest.NameQuestTable, DataQuest.ActiveListTask->GetNextListTask());
        GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
    }
}

void UQuestManager::NotifyUpdateQuest(const FName& QuestName)
{
    if (!UQuestLibrary::CheckIsClient(GetOwner()))
    {
        PushReplicateID(QuestName);
    }
    ClientSendNotifyUpdateQuest(QuestName);
}

#pragma endregion
