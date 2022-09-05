// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestManager.h"
#include "Librarys/QuestLibrary.h"
#include "ListTask/VisibleListTask.h"

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
    if (!CHECK_COND(DataQuestTable != nullptr, "Data quest table is nullptr")) return;

    FDataQuestTable* QuestFromTable = DataQuestTable->FindRow<FDataQuestTable>(QuestName, "");
    if (!CHECK_COND(QuestFromTable != nullptr, "Data quest from table is not found")) return;

    FDataQuest DataQuest(QuestName);
    DataQuest.ArrayDataListTask.Add(FDataVisibleListTask(QuestFromTable->StartVisibleListTask));
    ArrayDataQuest.AddUnique(DataQuest);

    FTimerHandle TempTimerHandle;
    FTimerDelegate TempTimerDelegate;
    TempTimerDelegate.BindUObject(this, &UQuestManager::StartInitQuest, QuestName);
    GetWorld()->GetTimerManager().SetTimer(TempTimerHandle, TempTimerDelegate, 0.1f, false);
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

void UQuestManager::ServerChangeTargetQuest_Implementation(const FName& QuestName)
{
    for (auto& Data : ArrayDataQuest)
    {
        Data.bIsTargetQuest = false;
    }
    
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    DataQuest.bIsTargetQuest = true;
    OnSwitchQuest.Broadcast(DataQuest.NameQuestTable);
    if (!UQuestLibrary::CheckIsClient(GetOwner()) && DataQuest.ActiveVisibleListTask)
    {
        PushReplicateID(DataQuest.ActiveVisibleListTask->GetUniqueID());
    }
}

bool UQuestManager::ServerChangeTargetQuest_Validate(const FName& QuestName)
{
    // TODO: Validate
    return true;
}

void UQuestManager::StartInitQuest(FName QuestName)
{
    if (!CHECK_COND(GetOwner()->HasAuthority(), "Owner is not Authority")) return;
    FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;
    if (!CHECK_COND(DataQuest.ArrayDataListTask.Num() == 1, "This is a broken set with more than 1 visible list task")) return;
    
    FDataVisibleListTask& DataVisibleListTask = DataQuest.ArrayDataListTask[0];
    UVisibleListTask* StartVisibleBlock = Cast<UVisibleListTask>(UQuestLibrary::CreateListTaskFromPath(GetOwner(), DataVisibleListTask.PathToVisibleListTask));
    if (!CHECK_COND(StartVisibleBlock != nullptr, "Start visible block is fail load in memory")) return;
    DataVisibleListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(StartVisibleBlock);
    DataQuest.ActiveVisibleListTask = StartVisibleBlock;
    DataQuest.StatusQuest = EStatusQuest::Init;
    
    OnStartQuest.Broadcast(DataQuest.NameQuestTable);
    if (!UQuestLibrary::CheckIsClient(GetOwner()))
    {
        PushReplicateID(StartVisibleBlock->GetUniqueID());
    }

    StartInitListTask(DataQuest.NameQuestTable);
}

void UQuestManager::StartInitListTask(const FName& QuestName)
{
    if (!CHECK_COND(GetOwner()->HasAuthority(), "Owner is not Authority")) return;
    const FDataQuest& DataQuest = GetDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;
    if (!CHECK_COND(DataQuest.ActiveVisibleListTask != nullptr, "Active visible list task is nullptr")) return;

    const bool bStatusInitListTask = DataQuest.ActiveVisibleListTask->InitListTask(Cast<APlayerController>(GetOwner()), this);
    if (!CHECK_COND(bStatusInitListTask, FString::Printf(TEXT("List task: [%s] is none init"),
        *DataQuest.ActiveVisibleListTask->GetName()))) return;

    const bool bStatusRunListTask = DataQuest.ActiveVisibleListTask->RunListTask();
    if (!CHECK_COND(bStatusRunListTask, FString::Printf(TEXT("List task: [%s] is none run"),
            *DataQuest.ActiveVisibleListTask->GetName()))) return;

    OnUpdateQuest.Broadcast(DataQuest.NameQuestTable);
    if (!UQuestLibrary::CheckIsClient(GetOwner()))
    {
        PushReplicateID(DataQuest.ActiveVisibleListTask->GetUniqueID());
    }
}

#pragma endregion

