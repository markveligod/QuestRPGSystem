﻿/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPG_QuestSystemDataTypes.h"
#include "RPG_QuestManagerBase.generated.h"

/**
 * @class Quest management system
 **/
UCLASS(DisplayName = "Quest Manager", ClassGroup = (Managers), meta = (BlueprintSpawnableComponent))
class RPG_QUESTSYSTEM_API URPG_QuestManagerBase : public UActorComponent
{
    GENERATED_BODY()

#pragma region Log

#define QUEST_MANAGER_LOG(LogVerb, Text) Print_LogQuestManger(LogVerb, Text, __LINE__, __FUNCTION__)
#define QUEST_MANAGER_CLOG(Cond, LogVerb, Text) Print_CLogQuestManager(Cond, LogVerb, Text, __LINE__, __FUNCTION__)

    /** @protected Write a log **/
    void Print_LogQuestManger(const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const;

    /** @protected Write a clog **/
    bool Print_CLogQuestManager(const bool bCond, const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const;

#pragma endregion

#pragma region DefaultInterface

public:
    URPG_QuestManagerBase();

    /** @public Called every frame **/
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** @public Defining rules for replication **/
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /**
     * Allows a component to replicate other subobject on the actor.
     * Must return true if any data gets serialized into the bunch.
     * This method is used only when bReplicateUsingRegisteredSubObjectList is false.
     * Otherwise this function is not called and only the ReplicatedSubObjects list is used.
     */
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

#pragma endregion

#pragma region ActionQuestManager

public:
    /** @public Request to the server about adding a new quest **/
    UFUNCTION(Server, Reliable, WithValidation, Category = "ActionQuestManager")
    void ServerAddQuest(const FName& NewQuest);

    /** @public Adding a new quest **/
    virtual void AddQuest(FName NewQuest);

    /** @public Request to the server about adding a new quest **/
    UFUNCTION(Server, Reliable, WithValidation, Category = "ActionQuestManager")
    void ServerRemoveQuest(const FName& QuestName);

    /** @public Adding a new quest **/
    virtual void RemoveQuest(FName QuestName);
    
    /** @public Search for quest data by its name **/
    FRPG_DataQuest* FindDataQuestByName(const FName& CheckQuest);

    /** @public Search for the index of quest data by its name **/
    int32 FindIndexDataQuestByName(const FName& CheckQuest) const;

    /** @public Search for quest data by quest object **/
    FRPG_DataQuest& FindDataQuestByQuestObject(URPG_QuestObjectBase* QuestObject);

    /** @public Search for quest data by quest object **/
    const FRPG_DataQuest& FindFreezeDataQuestByQuestObject(URPG_QuestObjectBase* QuestObject);

    /** @public Get quest list table **/
    UFUNCTION(BlueprintCallable)
    UDataTable* GetDataQuestTable() const { return DataQuestTable; }

    /** @public Get Status quest by name **/
    UFUNCTION(BlueprintCallable)
    ERPG_StateEntity GetStateQuestByName(FName QuestName);

protected:
    /** @protected Validation check for adding a quest**/
    virtual bool IsValidationRequestAddQuest(const FName& CheckQuest);

    /** @protected Validation check for remove a quest**/
    virtual bool IsValidationRequestRemoveQuest(const FName& CheckQuest);

    /** @protected Notification of data replication on the client **/
    UFUNCTION()
    virtual void OnRep_ArrayDataQuest();

    /** @protected Registering event: Quest data updates**/
    virtual void RegisterUpdateQuest_Event(URPG_QuestObjectBase* QuestObject);

    /** @protected Notification about updating the quest data **/
    virtual void NotifyUpdateQuest(FName NameQuest);

    /** @protected Destroy active quest **/
    virtual void DestroyActiveQuestByName(FName NameQuest);

#pragma endregion

#pragma region DataQuestManager

protected:
    /** @protected A set of quests**/
    UPROPERTY(ReplicatedUsing = OnRep_ArrayDataQuest)
    TArray<FRPG_DataQuest> ArrayDataQuest;

    /** @protected The owner of the component **/
    UPROPERTY()
    APlayerController* OwnerPC{nullptr};

    /** @protected Data quest table **/
    UPROPERTY(EditAnywhere)
    UDataTable* DataQuestTable{nullptr};

    /** @protected Empty quest data **/
    FRPG_DataQuest EmptyDataQuest;
#define EMPTY_DATA_QUEST EmptyDataQuest;

private:
    /** @private Queue of requests for replication of quests and its tasks **/
    TQueue<FName> QueueRepQuest;

#pragma endregion

#pragma region Signatures

private:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateQuestSignature, APlayerController*, Owner, const FName&, QuestName, const ERPG_StateEntity&, QuestState);

public:
    /** @signature Signature for the Quest status update event **/
    UPROPERTY(BlueprintAssignable)
    FUpdateQuestSignature OnUpdateDataQuest;

#pragma endregion
};
