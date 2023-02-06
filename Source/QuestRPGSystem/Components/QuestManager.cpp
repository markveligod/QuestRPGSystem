// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestManager.h"
#include "Librarys/QuestLibrary.h"

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
    
    
}

void UQuestManager::CompleteQuest(const FName QuestName)
{
    
}

void UQuestManager::InitListTask(const FName QuestName, const FSoftObjectPath ListTask)
{
    
}

void UQuestManager::RegisterUpdateListTask(UListTaskBase* ListTask)
{
   
}

void UQuestManager::NotifyUpdateQuest(const FName& QuestName)
{
    
}

#pragma endregion
