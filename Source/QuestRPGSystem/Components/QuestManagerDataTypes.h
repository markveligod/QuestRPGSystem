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
    NeedInit,
    Init,
    InProcess,
    CompleteSuccess,
    CompleteFail
};

UENUM()
enum class EStatePoint: uint8
{
    Draw,
    Remove
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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Quest", meta = (MetaClass = "ListTaskBase"))
    FSoftClassPath StartVisibleListTask;

    // Array Hidden list task
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Quest", meta = (MetaClass = "ListTaskBase"))
    TArray<FSoftClassPath> ArrayHiddenListTask;
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

    // Status hide task description
    UPROPERTY(BlueprintReadOnly)
    bool bHideTaskDescription{false};

    FDataInfoTask(){}
    FDataInfoTask(const FText& NewTaskDescription):TaskDescription(NewTaskDescription) {}
    FDataInfoTask(const FText& NewTaskDescription, const bool& bNewStatusTask, const bool& bNewHideTaskDescription):TaskDescription(NewTaskDescription),
    bStatusTask(bNewStatusTask), bHideTaskDescription(bNewHideTaskDescription){}
    FDataInfoTask(const FDataInfoTask& Other)
    {
        *this = Other;
    }

    void operator=(const FDataInfoTask& Other)
    {
        this->TaskID = Other.TaskID;
        this->TaskDescription = Other.TaskDescription;
        this->bStatusTask = Other.bStatusTask;
        this->bHideTaskDescription = Other.bHideTaskDescription;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Task Description: [%s] | bStatusTask: [%i] | bHideTaskDescription: [%i]"),
            *TaskDescription.ToString(), bStatusTask, bHideTaskDescription);
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
struct FDataVisibleListTask
{
    GENERATED_BODY()

    // Block of tasks that have been initialized
    UPROPERTY(BlueprintReadOnly)
    TArray<FDataInfoTask> ArrayDataTask;
    
    // Active current visible task
    UPROPERTY(BlueprintReadOnly, meta = (AllowedClasses = "ListTaskBase"))
    FSoftObjectPath PathToVisibleListTask{};

    UPROPERTY(BlueprintReadOnly)
    bool bListTaskComplete{false};

    FDataVisibleListTask(){}
    FDataVisibleListTask(const FSoftObjectPath& NewPathToListTask):PathToVisibleListTask(NewPathToListTask) {}
    FDataVisibleListTask(const FDataVisibleListTask& Other)
    {
        *this = Other;
    }

    void operator=(const FDataVisibleListTask& Other)
    {
        this->ArrayDataTask = Other.ArrayDataTask;
        this->PathToVisibleListTask = Other.PathToVisibleListTask;
        this->bListTaskComplete = Other.bListTaskComplete;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Path to Visible list task: [%s] | Array Data task: [%i] | List task complete: [%i]"),
            *PathToVisibleListTask.ToString(), ArrayDataTask.Num(), bListTaskComplete);
    }

    bool operator==(const FDataVisibleListTask& Other) const
    {
        return this->PathToVisibleListTask == Other.PathToVisibleListTask;
    }

    bool operator!=(const FDataVisibleListTask& Other) const
    {
        return !(this->PathToVisibleListTask == Other.PathToVisibleListTask);
    }
};

/**
 * @sturct Data List task for processing to hidden
 **/
USTRUCT(BlueprintType)
struct FDataHiddenListTask
{
    GENERATED_BODY()

    // Block of tasks that have been initialized
    UPROPERTY(BlueprintReadOnly)
    TArray<FDataInfoTask> ArrayDataTask;
    
    // Active current visible task
    UPROPERTY(BlueprintReadOnly, meta = (AllowedClasses = "ListTaskBase"))
    FSoftObjectPath PathToVisibleListTask{};

    UPROPERTY(BlueprintReadOnly)
    bool bListTaskComplete{false};

    UPROPERTY()
    class UListTaskBase* ActiveHiddenListTask{nullptr};

    FDataHiddenListTask(){}
    FDataHiddenListTask(const FSoftObjectPath& NewPathToListTask):PathToVisibleListTask(NewPathToListTask) {}
    FDataHiddenListTask(const FDataHiddenListTask& Other)
    {
        *this = Other;
    }

    void operator=(const FDataHiddenListTask& Other)
    {
        this->ArrayDataTask = Other.ArrayDataTask;
        this->PathToVisibleListTask = Other.PathToVisibleListTask;
        this->bListTaskComplete = Other.bListTaskComplete;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Path to Visible list task: [%s] | Array Data task: [%i] | List task complete: [%i]"),
            *PathToVisibleListTask.ToString(), ArrayDataTask.Num(), bListTaskComplete);
    }

    bool operator==(const FDataVisibleListTask& Other) const
    {
        return this->PathToVisibleListTask == Other.PathToVisibleListTask;
    }

    bool operator!=(const FDataVisibleListTask& Other) const
    {
        return !(this->PathToVisibleListTask == Other.PathToVisibleListTask);
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
    TArray<FDataVisibleListTask> ArrayDataListTask;

    // Active visible list task
    UPROPERTY()
    class UListTaskBase* ActiveVisibleListTask{nullptr};

    // Array Hidden list task
    UPROPERTY()
    TArray<FDataHiddenListTask> ArrayHiddenListTasks;

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
};

/**
 * @struct Data for drawing a point in the navigation system
 **/
USTRUCT(BlueprintType)
struct FTaskPoint
{
    GENERATED_BODY()

    // Pointer on actor
    UPROPERTY(BlueprintReadOnly)
    AActor* TargetActor{nullptr};

    // A backup option for installing a point
    UPROPERTY(BlueprintReadOnly)
    FVector AlternatePos{FVector::ZeroVector};

    // Static object or not
    UPROPERTY(BlueprintReadOnly)
    bool bStaticActor{false};

    // Deviation of the marker along the Z axis
    UPROPERTY(BlueprintReadOnly)
    float MarkDeviation{0.0f};

    // Quest Type
    UPROPERTY(BlueprintReadOnly)
    ETypeQuest QuestType{ETypeQuest::Story};

    // Tag for identification
    UPROPERTY(BlueprintReadOnly)
    FString Tag{};

    // Quest name
    UPROPERTY(BlueprintReadOnly)
    FText Head{};

    // Task description
    UPROPERTY(BlueprintReadOnly)
    FText Description{};

    // Name map
    UPROPERTY(BlueprintReadOnly)
    FName NameMap{};

    // Target quest or not
    UPROPERTY(BlueprintReadOnly)
    bool IsTargetQuest{false};

    // Is the pointer to the marker disabled
    UPROPERTY(BlueprintReadOnly)
    bool IsDetective{false};

    // Search radius
    UPROPERTY(BlueprintReadOnly)
    float RadiusSearch{0.0f};

    // Enable general zone
    UPROPERTY(BlueprintReadOnly)
    bool bEnableZone{false};

    // The central point of the zone
    UPROPERTY(BlueprintReadOnly)
    FVector CenterZone{};

    // Radius zone
    UPROPERTY(BlueprintReadOnly)
    float RadiusZone{0.0f};

    FString ToString() const
    {
        FString L_Result;
        L_Result.Append("Target Actor: " + (this->TargetActor != nullptr ? this->TargetActor->GetName() : "None") + "|");
        L_Result.Append("AlternatePos: " + this->AlternatePos.ToString() + "|");
        L_Result.Append("bStaticActor: " + FString::FromInt(this->bStaticActor) + "|");
        L_Result.Append("MarkDeviation: " + FString::SanitizeFloat(this->MarkDeviation) + "|");
        L_Result.Append("QuestType: " + UEnum::GetValueAsString(this->QuestType) + "|");
        L_Result.Append("Tag: " + this->Tag + "|");
        L_Result.Append("Head: " + this->Head.ToString() + "|");
        L_Result.Append("Description: " + this->Description.ToString() + "|");
        L_Result.Append("NameMap: " + this->NameMap.ToString() + "|");
        L_Result.Append("IsTargetQuest: " + FString::FromInt(this->IsTargetQuest) + "|");
        L_Result.Append("IsDetective: " + FString::FromInt(this->IsDetective) + "|");
        L_Result.Append("RadiusSearch: " + FString::SanitizeFloat(this->RadiusSearch) + "|");
        L_Result.Append("bEnableZone: " + FString::FromInt(this->bEnableZone) + "|");
        L_Result.Append("CenterZone: " + this->CenterZone.ToString() + "|");
        L_Result.Append("RadiusZone: " + FString::SanitizeFloat(this->RadiusZone));

        return L_Result;
    }
};

/**
 * @struct Data for send a quest point in the navigation system
 **/
USTRUCT(BlueprintType)
struct FQuestPointMap
{
    GENERATED_BODY()

    // Quest type
    UPROPERTY(BlueprintReadOnly)
    ETypeQuest QuestType{ETypeQuest::Story};

    // Tag
    UPROPERTY(BlueprintReadOnly)
    FString Tag{};  // GetParentQuest()->GetQuestName();

    // Quest name
    UPROPERTY(BlueprintReadOnly)
    FText Head{};  // GetParentQuest()->GetQuestName();

    // TaskDescription
    UPROPERTY(BlueprintReadOnly)
    FText Description{};  // TaskDescription(Last target)

    // GetMapName
    UPROPERTY(BlueprintReadOnly)
    FName NameMap{};  // Имя карты

    // Is target
    UPROPERTY(BlueprintReadOnly)
    bool IsTargetQuest{false};  // Нужно ли отображать этот квест (целевой или нет).

    // 
    UPROPERTY(BlueprintReadOnly)
    TArray<FTaskPoint> ArrTaskPoint;

    FString ToString() const
    {
        FString Result;
        Result.Append("Quest Type: " + UEnum::GetValueAsString(this->QuestType) + "|");
        Result.Append("Tag: " + this->Tag + "|");
        Result.Append("Head: " + this->Head.ToString() + "|");
        Result.Append("Description: " + this->Description.ToString() + "|");
        Result.Append("Name map: " + this->NameMap.ToString() + "|");
        Result.Append("Is target quest: " + FString::FromInt(this->IsTargetQuest));
        for (int32 i = 0; i < ArrTaskPoint.Num(); i++)
        {
            if (ArrTaskPoint.IsValidIndex(i))
            {
                Result.Append("Index Task point #" + FString::FromInt(i) + " " + ArrTaskPoint[i].ToString());
            }
        }

        return Result;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSendTaskPointSignature, FString, QuestName, FTaskPoint, TaskPoint, EStatePoint, StatePoint);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSendQuestPointMapSignature, FQuestPointMap, QuestPointMap);

