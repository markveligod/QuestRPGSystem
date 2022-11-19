// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/QuestManagerBase.h"
#include "QuestManager.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Managers), meta=(BlueprintSpawnableComponent))
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
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "API_Action")
    void ServerAddQuest(const FName& QuestName);

    /**
     * @public Request on server for add array new quest
     **/
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "API_Action")
    void ServerAddArrayQuest(const TArray<FName>& ArrayQuestName);

    /**
     * @public Request on server for change target quest
     **/
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "API_Action")
    void ServerChangeTargetQuest(const FName QuestName);
    
protected:

    /**
     * @protected Complete quest. Call on side server.
     **/
    virtual void CompleteQuest(const FName QuestName);

private:

    /**
     * @private The process of initial start initialization of a set of tasks.
     * The initial set is taken for initialization under the index 0
     **/
    void StartInitQuest(const FName QuestName);

    /**
      * @private Initial initialization of the task block
     **/
    void StartInitListTask(const FName& QuestName);

    /**
     * @private Registration updating of data on the task block
     **/
    void RegisterUpdateListTask(UListTaskBase* ListTask);

    /**
     * @private Next Initialize and run list task from Active List task for quest
     **/
    void NextListTask(const FName QuestName);

    /**
     * @private Notify update quest
     **/
    void NotifyUpdateQuest(const FName& QuestName);

    /**
     * @private Reject quest
     **/
    void RejectQuest(const FName& QuestName);
    
    /**
     * @protected Force next visible list task
     **/
    void InitNextVisibleListTask(const FName QuestName, const FSoftObjectPath NextVisibleLTPath);

#pragma endregion
    
};
