#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestManagerDataTypes.generated.h"

// 
UENUM(BlueprintType)
enum class ETypeQuest: uint8
{
    Story,
    Second,
    Event
};

// 
UENUM(BlueprintType)
enum class EStatusQuest: uint8
{
    HaveNot,
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

    // Type quest
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Quest")
    ETypeQuest TypeQuest{ETypeQuest::Story};

    // Start visible list task
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Quest", meta = (AllowedClasses = "VisibleListTask"))
    FSoftObjectPath StartVisibleListTask;
};

/**
 * @sturct Data  task for processing
 **/
USTRUCT(BlueprintType)
struct FDataTask
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Settings Data Task")
    FText TaskDescription{};
};

/**
 * @sturct Data List task for processing to visible
 **/
USTRUCT(BlueprintType)
struct FDataVisibleListTask
{
    GENERATED_BODY()

    // Block of tasks that have been initialized
    UPROPERTY(BlueprintReadOnly, Category = "Settings Data List Task")
    TArray<FDataTask> ArrayDataTask;
    
    // Active current visible task
    UPROPERTY(BlueprintReadOnly, Category = "Settings Data List Task", meta = (AllowedClasses = "VisibleListTask"))
    FSoftObjectPath PathToVisibleListTask{};

    UPROPERTY(BlueprintReadOnly, Category = "Settings Data List Task")
    bool bListTaskComplete{false};
};

/**
 * @sturct Data quest for processing
 **/
USTRUCT(BlueprintType)
struct FDataQuest 
{
    GENERATED_BODY()

    // Status quest
    UPROPERTY(BlueprintReadOnly, Category = "Settings Quest")
    EStatusQuest StatusQuest{EStatusQuest::InProcess};

    // Name quest from table
    UPROPERTY(BlueprintReadOnly, Category = "Settings Quest")
    FName NameQuestTable{};
    
    // Name quest from table
    UPROPERTY(BlueprintReadOnly, Category = "Settings Quest")
    bool bIsTargetQuest{false};

    // Array data to visible list task
    UPROPERTY(BlueprintReadOnly, Category = "Settings Quest")
    TArray<FDataVisibleListTask> ArrayDataListTask;

    // Active visible list task
    UPROPERTY()
    class UVisibleListTask* ActiveVisibleListTask{nullptr};
};

