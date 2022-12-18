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
     * @public Request on server for init new list task
     **/
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "API_Action")
    void ServerInitListTask(const FName& QuestName, const FSoftClassPath& ListTaskPath);

protected:

    /**
     * @protected Add quest. Call on side server.
     **/
    virtual void AddQuest(const FName& QuestName);

    /**
     * @protected Checked add quest. Call on side server.
     **/
    virtual bool CheckedAddQuest(const FName& QuestName);

    /**
     * @protected Checked add array quest. Call on side server.
     **/
    virtual bool CheckedAddArrayQuest(const TArray<FName>& ArrayQuestName);

    /**
     * @protected Checked to init list task. Call on side server.
     **/
    virtual bool CheckedInitListTask(const FName& QuestName, const FSoftClassPath& ListTaskPath);

    /**
     * @protected Complete quest. Call on side server.
     **/
    virtual void CompleteQuest(const FName QuestName);

    /**
     * @protected Registration updating of data on the task block
     **/
    virtual void RegisterUpdateListTask(UListTaskBase* ListTask);

    /**
     * @protected Initialization of the task block
     **/
    virtual void InitListTask(const FName QuestName, const FSoftObjectPath ListTask);

    /**
     * @protected Notify update quest
     **/
    virtual void NotifyUpdateQuest(const FName& QuestName);

#pragma endregion
};
