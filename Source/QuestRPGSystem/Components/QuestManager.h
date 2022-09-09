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
    void ServerChangeTargetQuest(const FName QuestName);

    /**
     * @public Add a new task sheet to the quest
     * @param1 FName
     * @param2 FSoftClassPath&
     **/
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "API_Action")
    void ServerAddHiddenListTask(const FName& QuestName, const FSoftClassPath& HiddenListTaskPath);

protected:

    /**
     * @protected Complete quest. Call on side server.
     * @param1 FName
     **/
    virtual void CompleteQuest(const FName QuestName);

private:

    /**
     * @private The process of initial start initialization of a set of tasks.
     * The initial set is taken for initialization under the index 0
     * @param1 FName - Name quest
     **/
    void StartInitQuest(const FName QuestName);

    /**
      * @private Initial initialization of the task block
      * @param1 FName - Name quest
     **/
    void StartInitListTask(const FName& QuestName);

    /**
     * @private Registration updating of data on the task block
     * @param1 UListTaskBase*
     **/
    void RegisterUpdateListTask(UListTaskBase* ListTask);

    /**
     * @private Next Initialize and run list task from Active List task for quest
     * @param1 FName
     **/
    void NextListTask(const FName QuestName);

    /**
     * @private Notify update quest
     * @param1 FName NameQuest
     **/
    void NotifyUpdateQuest(const FName& QuestName);

    /**
     * @private Reject quest
     * @param1 FName Name quest
     **/
    void RejectQuest(const FName& QuestName);
    
    /**
     * @protected Force next visible list task
     * @param1 FName
     * @param2 FSoftObjectPath
     **/
    void InitNextVisibleListTask(const FName QuestName, const FSoftObjectPath NextVisibleLTPath);

#pragma endregion

#pragma region HiddenListTaskProcess

protected:

    /**
     * @protected Check initialize hidden list task
     * @param1 FName
     **/
    void CheckedInitHiddenListTask(const FName& QuestName);

    /**
     * @protected Check to run hidden list task
     * @param1 FName
     **/
    void CheckedRunHiddenListTask(const FName& QuestName);

private:
    
    /**
     * @protected Register update hidden list task
     * @param1 UListTaskBase*
     **/
    void RegisterUpdateHiddenListTask(UListTaskBase* HiddenLT);

#pragma endregion
    
};
