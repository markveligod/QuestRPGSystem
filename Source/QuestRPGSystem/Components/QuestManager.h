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
     * @param1 FName
     **/
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "API_Action")
    void ServerAddQuest(const FName& QuestName);

    /**
     * @public Request on server for add array new quest
     * @param1 TArray<FName>
     **/
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "API_Action")
    void ServerAddArrayQuest(const TArray<FName>& ArrayQuestName);

    /**
     * @public Request on server for change target quest
     * @param1 FName
     **/
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "API_Action")
    void ServerChangeTargetQuest(const FName& QuestName);

private:

    /**
     * @private The process of initial start initialization of a set of tasks.
     * The initial set is taken for initialization under the index 0
     * @param1 FName - Name quest
     **/
    void StartInitQuest(FName QuestName);

    /**
      * @private Initial initialization of the task block
      * @param1 FName - Name quest
     **/
    void StartInitListTask(const FName& QuestName);

#pragma endregion
    
};
