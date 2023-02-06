﻿#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestManagerDataTypes.generated.h"

/** Call for start quest in Quest Manager **/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartQuestSignature, const FName&, NameQuest);

/** Call for update quest in Quest Manager **/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateQuestSignature, const FName&, NameQuest);

/** Call for complete quest in Quest Manager **/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCompleteQuestSignature, const FName&, NameQuest);

/** @enum Status process quest **/
UENUM(BlueprintType)
enum class EStatusQuest: uint8
{
    HaveNot,
    Start,
    InProcess,
    Complete,
};

/** @struct Data quest for Table **/
USTRUCT(BlueprintType)
struct FDataQuestTable : public FTableRowBase
{
    GENERATED_BODY()

    // Name quest
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Quest")
    FText NameQuest{};

    // Description quest
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Quest")
    FText DescriptionQuest{};

    // Start visible list task
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Quest", meta = (MetaClass = "QuestObject"))
    FSoftClassPath QuestObject;
};

/** @sturct Data task for processing **/
USTRUCT(BlueprintType)
struct FDataInfoTask
{
    GENERATED_BODY()

    // Id task
    UPROPERTY(BlueprintReadOnly, SaveGame)
    FString TaskID{};

    // Task description
    UPROPERTY(BlueprintReadOnly, SaveGame)
    FText TaskDescription{};

    // status task
    UPROPERTY(BlueprintReadOnly, SaveGame)
    bool bStatusTask{false};

    FDataInfoTask(){}
    FDataInfoTask(const FText& NewTaskDescription):TaskDescription(NewTaskDescription) {}
    FDataInfoTask(const FText& NewTaskDescription, const bool& bNewStatusTask, const bool& bNewHideTaskDescription):TaskDescription(NewTaskDescription),
    bStatusTask(bNewStatusTask) {}
    FDataInfoTask(const FDataInfoTask& Other)
    {
        *this = Other;
    }

    void operator=(const FDataInfoTask& Other)
    {
        this->TaskID = Other.TaskID;
        this->TaskDescription = Other.TaskDescription;
        this->bStatusTask = Other.bStatusTask;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Task Description: [%s] | bStatusTask: [%i] "),
            *TaskDescription.ToString(), bStatusTask);
    }

    bool operator==(const FDataInfoTask& Other) const
    {
        return this->TaskID == Other.TaskID;
    }

    bool operator!=(const FDataInfoTask& Other) const
    {
        return this->TaskID != Other.TaskID;
    }
};

/**
 * @sturct Data quest for processing
 **/
USTRUCT(BlueprintType)
struct FDataQuest 
{
    GENERATED_USTRUCT_BODY()

    // Status quest
    UPROPERTY(BlueprintReadOnly, SaveGame)
    EStatusQuest StatusQuest{EStatusQuest::Start};

    // Name quest from table
    UPROPERTY(BlueprintReadOnly, SaveGame)
    FName NameQuestTable{};

    // Array data info task
    UPROPERTY(BlueprintReadOnly, SaveGame)
    TArray<FDataInfoTask> ArrayDataListTask;

    // Active visible list task
    UPROPERTY()
    class UQuestObject* ActiveObject{nullptr};

    FDataQuest(){}
    explicit FDataQuest(const FName& NewNameQuest): NameQuestTable(NewNameQuest){}

    FString ToString() const
    {
        return FString::Printf(TEXT("Name Quest Table: [%s] | StatusQuest: [%s] | ArrayDataListTask: [%i]"),
            *NameQuestTable.ToString(), *UEnum::GetValueAsString(StatusQuest), ArrayDataListTask.Num());
    }

    bool operator==(const FDataQuest& Other) const
    {
        return NameQuestTable.IsEqual(Other.NameQuestTable);
    }

    bool operator!=(const FDataQuest& Other) const
    {
        return !(NameQuestTable.IsEqual(Other.NameQuestTable));
    }

    bool IsEmpty() const
    {
        return this->NameQuestTable.IsNone();
    }
};

USTRUCT(BlueprintType)
struct FDrawDebugQuestData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    FString ToString() const
    {
        return FString::Printf(TEXT("Name: [%s]\nPosition: [%s]"), *Name, *Position.ToString());
    }
};
