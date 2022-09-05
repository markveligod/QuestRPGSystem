// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestManagerDataTypes.h"
#include "Librarys/QuestLibrary.h"
#include "QuestManagerBase.generated.h"

#define LOG_QM(LogVerb, Text) Print_LogQuestManager(LogVerb, Text, __LINE__, __FUNCTION__)

class UListTaskBase;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class QUESTRPGSYSTEM_API UQuestManagerBase : public UActorComponent
{
    GENERATED_BODY()

#pragma region LogQuestManager

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
     * @param1 ID
     **/
    void PushReplicateID(const uint32 ID);

private:

    UFUNCTION()
    void OnRep_AddDataQuest();

#pragma endregion

#pragma region FindData

public:

    /**
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
     * @protected 
     **/
    FDataQuest& GetDataQuestFromName(const FName& NameQuest);

#pragma endregion

#pragma region DataQuestManager

protected:

    // @protected Current array data quest
    UPROPERTY(Replicated, ReplicatedUsing = OnRep_AddDataQuest)
    TArray<FDataQuest> ArrayDataQuest;

    // @protected Data quest table
    UPROPERTY(EditAnywhere, Category = "Data Quest Manager")
    UDataTable* DataQuestTable;

    TQueue<uint32> QueuePushReplicateObject;

    FDataQuest EmptyDataQuest;

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
