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

#pragma region Action

public:

    /**
     * @public Adds a unique ID of the task list object to the queue
     * @param1 FName
     **/
    void PushReplicateID(const FName& QuestName);

protected:

    /**
     * @protected Update Info data quest from active list task
     * @param1 FName
     **/
    void UpdateInfoDataQuest(const FName& NameQuest);

    /**
     * @protected Client notify start quest
     * @param1 FName name quest
     **/
    UFUNCTION(Client, Reliable)
    void ClientSendNotifyStartQuest(const FName& NameQuest);

    /**
     * @protected Client notify update quest
     * @param1 FName name quest
     **/
    UFUNCTION(Client, Reliable)
    void ClientSendNotifyUpdateQuest(const FName& NameQuest);

    /**
     * @protected Client notify complete quest
     * @param1 FName name quest
     **/
    UFUNCTION(Client, Reliable)
    void ClientSendNotifyCompleteQuest(const FName& NameQuest);

    /**
     * @protected Client notify switch quest
     * @param1 FName name quest
     **/
    UFUNCTION(Client, Reliable)
    void ClientSendNotifySwitchQuest(const FName& NameQuest);

#pragma endregion

#pragma region FindData

public:

    /**
     * @public Get constant data quest from Name quest
     * @param1 FName NameQuest
     * @return const FDataQuest&
     **/
    UFUNCTION(BlueprintCallable, Category = "FindData")
    const FDataQuest& GetFreezeDataQuestFromName(const FName& NameQuest) const;
    
protected:

    /**
     * @protected Get class UListTaskBase from unique ID UObject
     * @param1 uint32
     * @return UListTaskBase*
     **/
    UListTaskBase* FindListTaskFromID(const uint32 ID) const;

    /**
     * @protected Get data quest from unique ID UObject Active list task
     * @param1 uint32
     * @return FDataQuest&
     **/
    FDataQuest& FindDataQuestFromID(const uint32 ID) ;

    /**
     * @public Get non-constant data quest from Name quest
     * @param1 FName NameQuest
     * @return FDataQuest&
     **/
    FDataQuest& GetDataQuestFromName(const FName& NameQuest);

    /**
     * @public Get non-constant data quest from Active list task
     * @param1 UListTaskBase*
     * @return FDataQuest&
     **/
    FDataQuest& GetDataQuestFromListTask(const UListTaskBase* ListTask);

    /**
     * @public Get non-constant DataVisibleListTask from Active list task
     * @param1 UListTaskBase*
     * @return FDataVisibleListTask&
     **/
    FDataVisibleListTask& GetDataVisibleListFromListTask(const FName& NameQuest, const UListTaskBase* ListTask);

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

    FDataQuest EmptyDataQuest;
    FDataVisibleListTask EmptyDataVisibleListTask;

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

    UPROPERTY(BlueprintAssignable)
    FSendTaskPointSignature OnSendTaskPoint;

    UPROPERTY(BlueprintAssignable)
    FSendQuestPointMapSignature OnSendQuestPointMap;

#pragma endregion
    
};
