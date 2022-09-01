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

void UQuestManager::ServerAddNewQuest_Implementation(const FName& QuestName)
{
    if (!CHECK_COND(DataQuestTable != nullptr, "Data quest table is nullptr")) return;
    
}

#pragma endregion

