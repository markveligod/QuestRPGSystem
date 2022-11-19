// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskDataTypes.h"
#include "QuestRPGSystem/Librarys/QuestLibrary.h"
#include "TaskBase.generated.h"

class UListTaskBase;

#define LOG_TASK(LogVerb, Text) Print_LogTask(LogVerb, Text, __LINE__, __FUNCTION__)

/**
 * @class Basic class settings for the implementation of the approach to the implementation of the task entity
 */
UCLASS(EditInlineNew, Abstract, HideCategories = "Trash")
class QUESTRPGSYSTEM_API UTaskBase : public UObject
{
    GENERATED_BODY()

friend class UListTaskBase;

#pragma region LogTask

protected:
    
    /**
     * @public Write a log to task
     **/
    void Print_LogTask(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const;

#pragma endregion

#pragma region Default

public:
    
    // Construct
    UTaskBase()
    {
    }

    /**
     * @public Call on server side for init task. this override function.
     * @param1 APlayerController*
     * @param2 UListTaskBase*
     * @return bool
     **/
    virtual bool InitTask(APlayerController* PlayerController, UListTaskBase* Parent);

    /**
     * @public Call on server side for reset task. this override function.
     * @return bool
     **/
    virtual bool ResetTask();

    /**
     * @public Call on client side for run task. this override function.
     * @return bool
     **/
    virtual bool RunTask();

    /**
     * @public Call on server side for complete task. this override function.
     * @return bool
     **/
    virtual bool CompleteTask();

    /**
     * @public Call on server side for check valid task. this override function.
     * @return bool
     **/
    virtual bool IsValidTask();

#pragma endregion

#pragma region DataTask

public:

    /**
     * @public Get task description
     * @return FText
     **/
    FORCEINLINE FText GetTaskDescription() const { return TaskDescription; }

    /**
     * @public Get state bNotifyUpdate
     * @return bool
     **/
    FORCEINLINE bool IsNotifyUpdate() const { return bNotifyUpdate; }

    /**
     * @public Get Task Specific Settings
     * @return FTaskSpecificSettings
     **/
    FORCEINLINE const FTaskSpecificSettings& GetTaskSpecificSettings() { return TaskSpecificSettings; }

    /**
     * @public Get StatusTask
     * @return EStatusTask
     **/
    FORCEINLINE EStatusTask GetStatusTask() const { return StatusTask; }

    /**
     * @public Get state edit task
     * @return bool
     **/
    FORCEINLINE bool IsEditBaseSettingsTask() const { return bEditBaseSettingsTask; }

protected:
    
    // @protected Parameter for disabling task settings
    UPROPERTY(VisibleAnywhere, Category = "Trash")
    bool bEditBaseSettingsTask = true;

    // @protected Task description
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Task", meta = (DisplayPriority = "0", EditCondition = "bEditBaseSettingsTask", EditConditionHides))
    FText TaskDescription{};

    // @protected Notification of the quest update at the start of a current task
    UPROPERTY(EditDefaultsOnly, Category = "Settings Task", meta = (DisplayPriority = 1, EditCondition = "bEditBaseSettingsTask", EditConditionHides))
    bool bNotifyUpdate{true};

    // @protected Task specific settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Task", meta = (EditCondition = "bEditBaseSettingsTask", EditConditionHides))
    FTaskSpecificSettings TaskSpecificSettings{};

    // @protected Status task
    EStatusTask StatusTask{EStatusTask::NoneInit};

    // @protected Owner player controller
    UPROPERTY()
    APlayerController* OwnerController{nullptr};

    // @protected Owner Task
    UPROPERTY()
    UListTaskBase* OwnerListTask{nullptr};

#pragma endregion

#pragma region ActionBase

public:

    /**
     * @public To change status task
     * @param1 EStatusTask
     **/
    void ChangeStatusTask(const EStatusTask& NewStatus);

    /**
     * @public Checking whether the tasks are completed
     * @return bool
     **/
    bool IsTaskComplete() const { return StatusTask == EStatusTask::Complete; }

protected:

    // TODO Temp function for test
    UFUNCTION(Client, Reliable)
    void ClientDrawPoint(const FVector& Position);

#pragma endregion

#pragma region Signature

private:
    
    // @public Notify for paren list task
    FUpdateTaskSignature OnUpdateTask;

#pragma endregion
    
};
