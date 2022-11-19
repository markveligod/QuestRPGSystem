#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestManagerDataTypes.generated.h"

/**
 * Call for start quest in Quest Manager
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartQuestSignature, const FName&, NameQuest);

/**
 * Call for update quest in Quest Manager
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateQuestSignature, const FName&, NameQuest);

/**
 * Call for complete quest in Quest Manager
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCompleteQuestSignature, const FName&, NameQuest);

/**
 * Call for switch target quest in Quest Manager
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSwitchQuestSignature, const FName&, NameQuest);

// @enum Status process quest
UENUM(BlueprintType)
enum class EStatusQuest: uint8
{
    HaveNot,
    NeedInit,
    Init,
    InProcess,
    CompleteSuccess,
    CompleteFail
};

/**
 * @sturct Data quest for Table
 **/
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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Quest", meta = (MetaClass = "ListTaskBase"))
    FSoftClassPath StartListTask;
};

/**
 * @sturct Data task for processing
 **/
USTRUCT(BlueprintType)
struct FDataInfoTask
{
    GENERATED_BODY()

    // Id task
    UPROPERTY()
    FString TaskID{};
    
    // Task description
    UPROPERTY(BlueprintReadOnly)
    FText TaskDescription{};

    // status task
    UPROPERTY(BlueprintReadOnly)
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
        return !(this->TaskID == Other.TaskID);
    }
};

/**
 * @sturct Data List task for processing to visible
 **/
USTRUCT(BlueprintType)
struct FDataListTask
{
    GENERATED_BODY()

    // Block of tasks that have been initialized
    UPROPERTY(BlueprintReadOnly)
    TArray<FDataInfoTask> ArrayDataTask;
    
    // Active current visible task
    UPROPERTY(BlueprintReadOnly, meta = (AllowedClasses = "ListTaskBase"))
    FSoftObjectPath PathToListTask{};

    UPROPERTY(BlueprintReadOnly)
    bool bListTaskComplete{false};

    FDataListTask(){}
    FDataListTask(const FSoftObjectPath& InPathToListTask):PathToListTask(InPathToListTask) {}
    FDataListTask(const FDataListTask& Other)
    {
        *this = Other;
    }

    void operator=(const FDataListTask& Other)
    {
        this->ArrayDataTask = Other.ArrayDataTask;
        this->PathToListTask = Other.PathToListTask;
        this->bListTaskComplete = Other.bListTaskComplete;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Path to Visible list task: [%s] | Array Data task: [%i] | List task complete: [%i]"),
            *PathToListTask.ToString(), ArrayDataTask.Num(), bListTaskComplete);
    }

    bool operator==(const FDataListTask& Other) const
    {
        return this->PathToListTask == Other.PathToListTask;
    }

    bool operator!=(const FDataListTask& Other) const
    {
        return !(this->PathToListTask == Other.PathToListTask);
    }

    bool IsEmpty() const
    {
        return this->PathToListTask.IsNull();
    }
};

/**
 * @sturct Data quest for processing
 **/
USTRUCT(BlueprintType)
struct FDataQuest 
{
    GENERATED_BODY()

    // Status quest
    UPROPERTY(BlueprintReadOnly)
    EStatusQuest StatusQuest{EStatusQuest::NeedInit};

    // Name quest from table
    UPROPERTY(BlueprintReadOnly)
    FName NameQuestTable{};
    
    // Name quest from table
    UPROPERTY(BlueprintReadOnly)
    bool bIsTargetQuest{false};

    // Array data to visible list task
    UPROPERTY(BlueprintReadOnly)
    TArray<FDataListTask> ArrayDataListTask;

    // Active visible list task
    UPROPERTY()
    class UListTaskBase* ActiveListTask{nullptr};

    FDataQuest(){}
    explicit FDataQuest(const FName& NewNameQuest): NameQuestTable(NewNameQuest){}

    FString ToString() const
    {
        return FString::Printf(TEXT("Name Quest Table: [%s] | StatusQuest: [%s] | bIsTargetQuest: [%i] | ArrayDataListTask: [%i]"),
            *NameQuestTable.ToString(), *UEnum::GetValueAsString(StatusQuest), bIsTargetQuest, ArrayDataListTask.Num());
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

