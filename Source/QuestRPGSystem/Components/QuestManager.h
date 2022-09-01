// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/QuestManagerBase.h"
#include "QuestManager.generated.h"

/**
 * 
 */
UCLASS()
class QUESTRPGSYSTEM_API UQuestManager : public UQuestManagerBase
{
	GENERATED_BODY()

#pragma region API_Checked

public:

    UFUNCTION(BlueprintCallable, Category = "QuestManager | API_Checked")
    EStatusQuest GetStatusQuest(const FName& QuestName);

#pragma endregion

#pragma region API_Action

public:

    /**
     * @public Request on server for add new quest
     **/
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "QuestManager | API_Action")
    void ServerAddNewQuest(const FName& QuestName);

#pragma endregion
    
};
