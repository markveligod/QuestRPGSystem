// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestManagerDataTypes.h"
#include "Librarys/QuestLibrary.h"
#include "QuestManagerBase.generated.h"

#define LOG_QM(LogVerb, Text) Print_LogQuestManager(LogVerb, Text, __LINE__, __FUNCTION__)

class UListTaskBase;
UCLASS()
class QUESTRPGSYSTEM_API UQuestManagerBase : public UActorComponent
{
    GENERATED_BODY()

#pragma region LogQuestManager

protected:
    
    /**
     * @public Write a log to quest manager
     **/
    void Print_LogQuestManager(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const;

#pragma endregion

#pragma region Default
    
public:

    // Sets default values for this component's properties
    UQuestManagerBase();

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // @public Defining rules for replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // @public Allows a component to replicate other subobject on the actor
    virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
    
protected:
    
    // Called when the game starts
    virtual void BeginPlay() override;

#pragma endregion

#pragma region ActionBase

public:

    /**
     * @public Adds a unique ID of the task list object to the queue
     **/
    void PushReplicateID(const FName& QuestName);

    /**
     * @public Get array data quest
     **/
    UFUNCTION(BlueprintCallable, Category = "ActionBase")
    const TArray<FDataQuest>& GetArrayDataQuest() { return ArrayDataQuest; }

    /**
     * @public Get array data quest
     **/
    UFUNCTION(BlueprintCallable, Category = "ActionBase")
    FString GetJSONFromArrayDataQuest();

    /**
     * @public Setup array data quest from JSON string
     **/
    UFUNCTION(BlueprintCallable, Category = "ActionBase")
    void SetupNewArrayDataQuestFromJSON(const FString& JSONString) {}

    /**
     * @public Add array new data quest
     **/
    UFUNCTION(BlueprintCallable, Category = "ActionBase")
    void AddNewArrayDataQuest(const TArray<FDataQuest>& NewData);

protected:

    /**
     * @protected Update Info data quest from active list task
     **/
    void UpdateInfoDataQuest(const FName& NameQuest);

    /**
     * @protected Client notify start quest
     **/
    UFUNCTION(Client, Reliable)
    void ClientSendNotifyStartQuest(const FName& NameQuest);

    /**
     * @protected Client notify update quest
     **/
    UFUNCTION(Client, Reliable)
    void ClientSendNotifyUpdateQuest(const FName& NameQuest);

    /**
     * @protected Client notify complete quest
     **/
    UFUNCTION(Client, Reliable)
    void ClientSendNotifyCompleteQuest(const FName& NameQuest);

    /**
     * @protected Client notify switch quest
     **/
    UFUNCTION(Client, Reliable)
    void ClientSendNotifySwitchQuest(const FName& NameQuest);

    /**
     * @protected notify start quest
     **/
    virtual void SendNotifyStartQuest(const FName& NameQuest);

    /**
     * @protected notify update quest
     **/
    virtual void SendNotifyUpdateQuest(const FName& NameQuest);

    /**
     * @protected notify complete quest
     **/
    virtual void SendNotifyCompleteQuest(const FName& NameQuest);

    /**
     * @protected notify switch quest
     **/
    virtual void SendNotifySwitchQuest(const FName& NameQuest);

    /**
     * @protected notify start quest event
     **/
    UFUNCTION(BlueprintImplementableEvent)
    void SendNotifyStartQuest_Event(const FName& NameQuest);

    /**
     * @protected notify update quest event
     **/
    UFUNCTION(BlueprintImplementableEvent)
    void SendNotifyUpdateQuest_Event(const FName& NameQuest);

    /**
     * @protected notify complete quest event
     **/
    UFUNCTION(BlueprintImplementableEvent)
    void SendNotifyCompleteQuest_Event(const FName& NameQuest);

    /**
     * @protected notify switch quest event
     **/
    UFUNCTION(BlueprintImplementableEvent)
    void SendNotifySwitchQuest_Event(const FName& NameQuest);

    /**
      * @protected Destruction of an active list of tasks for a specific quest
     **/
    void DestroyActiveListTaskFromQuestName(const FName& NameQuest);

    /**
      * @protected Destruction of an active list of tasks for all quest
     **/
    void DestroyActiveListTaskForAllQuest();

#pragma endregion

#pragma region FindData

public:

    /**
     * @public Get index from Array quest data
     **/
    UFUNCTION(BlueprintCallable, Category = "FindData")
    int32 GetIndexQuestFromName(const FName& InQuestName);

    /**
     * @public Get constant data quest from index
     **/
    UFUNCTION(BlueprintCallable, Category = "FindData")
    const FDataQuest& GetFreezeDataQuestFromIndex(const int32& InIndex);

    /**
     * @public Get constant data quest from Name quest
     **/
    UFUNCTION(BlueprintCallable, Category = "FindData")
    const FDataQuest& GetFreezeDataQuestFromName(const FName& InQuestName);

    /**
     * @public Get dat quest table from name
     **/
    UFUNCTION(BlueprintCallable, Category = "FindData")
    FDataQuestTable GetDataQuestTableFromName(const FName& NameQuest) const;
    
protected:

    /**
     * @protected Get class UListTaskBase from unique ID UObject
     **/
    UListTaskBase* FindListTaskFromID(const uint32 ID) const;

    /**
     * @protected Get data quest from unique ID UObject Active list task
     **/
    FDataQuest& FindDataQuestFromID(const uint32 ID);

    /**
     * @protected Get non-constant data quest from Name quest
     **/
    FDataQuest& GetDataQuestFromName(const FName& NameQuest);

    /**
     * @protected Get non-constant data quest from Active list task
     **/
    FDataQuest& GetDataQuestFromListTask(const UListTaskBase* ListTask);

    /**
     * @protected Get non-constant data quest from list task path
     **/
    FDataQuest& GetDataQuestFromPathListTask(const FSoftObjectPath& InListTaskPath);

    /**
     * @protected Get non-constant data list task from list task path
     **/
    FDataListTask& GetDataListTaskFromPathListTask(const FSoftObjectPath& InListTaskPath);

#pragma endregion

#pragma region DataQuestManager

protected:

    // @protected Current array data quest
    UPROPERTY(Replicated)
    TArray<FDataQuest> ArrayDataQuest;

    // @protected Data quest table
    UPROPERTY(EditAnywhere, Category = "Data Quest Manager")
    UDataTable* DataQuestTable;

    TQueue<uint32> QueuePushReplicateObject;

    // @protected Constant empty data quest
    FDataQuest EmptyDataQuest;

    // @protected Constant empty data list task
    FDataListTask EmptyDataListTask;

#pragma endregion

#pragma region Signature

    UPROPERTY(BlueprintAssignable)
    FStartQuestSignature OnStartQuest;

    UPROPERTY(BlueprintAssignable)
    FUpdateQuestSignature OnUpdateQuest;

    UPROPERTY(BlueprintAssignable)
    FCompleteQuestSignature OnCompleteQuest;

    UPROPERTY(BlueprintAssignable)
    FSwitchQuestSignature OnSwitchQuest;

#pragma endregion
    
};
