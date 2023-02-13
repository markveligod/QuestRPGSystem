// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskDataTypes.h"
#include "QuestRPGSystem/Librarys/QuestLibrary.h"
#include "TaskBase.generated.h"

class UQuestObject;

#define LOG_TASK(LogVerb, Text) Print_LogTask(LogVerb, Text, __LINE__, __FUNCTION__)

/**
 * @class Basic class settings for the implementation of the approach to the implementation of the task entity
 */
UCLASS(EditInlineNew, DefaultToInstanced, Abstract, Blueprintable)
class QUESTRPGSYSTEM_API UTaskBase : public UObject
{
    GENERATED_BODY()

    friend class UListTaskBase;

#pragma region LogTask

protected:

    /**
     * @protected Write a log to task
     **/
    virtual void Print_LogTask(const TEnumAsByte<EQuestLogVerb> LogVerb, const FString Text, const int Line, const char* Function) const;

#pragma endregion

#pragma region Default

public:

    /** Construct **/
    UTaskBase()
    {
    }

protected:

    /** @protected Call on server side for init task  **/
    virtual bool InitTask(APlayerController* PlayerController, UQuestObject* Parent);

    /** @protected Call on server side for init task **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    bool InitTask_Event(APlayerController* PlayerController, UQuestObject* Parent);

    /** @protected Call on server side for reset task **/
    virtual bool ResetTask();

    /** @protected Call on server side for reset task **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    bool ResetTask_Event();

    /** @protected Call on client side for run task **/
    virtual bool RunTask();

    /** @protected Call on server side for run task **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    bool RunTask_Event();

    /** @protected Call on server side for complete task **/
    virtual bool CompleteTask();

    /** @protected Call on server side for complete task **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    bool CompleteTask_Event();

    /** @protected Call on server side for check valid task **/
    virtual bool IsValidTask();

    /** @protected Call on server side for check valid task **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    bool IsValidTask_Event();

    /** @protected Call on server side for check abort task **/
    virtual bool IsAbortTask();

    /** @protected Call on server side for check abort task **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    bool IsAbortTask_Event();

    /** @protected Request on server for complete task **/
    UFUNCTION(Server, Reliable, BlueprintCallable, meta = (BlueprintProtected = true))
    void ServerCompleteTask();

    /**
     * @public Return the space this function should be called. Checks to see if this function should
     * be called locally, remotely, or simply absorbed under the given conditions
     *
     * @param Function function to call
     * @param Stack stack frame for the function call
     * @return bitmask representing all callspaces that apply to this UFunction in the given context
     */
    virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;

    /**
     * @public Call the actor's function remotely
     *
     * @param1 Function function to call
     * @param2 Parameters arguments to the function call
     * @param3 Stack stack frame for the function call
     */
    virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;

    /** @public Returns properties that are replicated for the lifetime of the actor channel */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** @public IsSupportedForNetworking means an object can be referenced over the network */
    virtual bool IsSupportedForNetworking() const override { return true; }

#pragma endregion

#pragma region DataTask

public:

    /** @public Get task description **/
    FORCEINLINE FText GetTaskDescription() const { return FText::FromString(TaskDescription.ToString() + " " + AddDescription + " " + TimerDescription); }

    /** @public Get state bNotifyUpdate **/
    FORCEINLINE bool IsNotifyUpdate() const { return bNotifyUpdate; }

    /**
     * @public Get Task Specific Settings
     **/
    FORCEINLINE const FTaskSpecificSettings& GetTaskSpecificSettings() { return TaskSpecificSettings; }

    /**
     * @public Get StatusTask
     **/
    FORCEINLINE EStatusTask GetStatusTask() const { return StatusTask; }

protected:
    // @protected Task description
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Task", meta = (DisplayPriority = "0"))
    FText TaskDescription{};

    // @protected Notification of the quest update at the start of a current task
    UPROPERTY(EditAnywhere, Category = "Settings Task", meta = (DisplayPriority = 1))
    bool bNotifyUpdate{true};

    // @protected Task specific settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Task")
    FTaskSpecificSettings TaskSpecificSettings{};

    // @protected Status task
    EStatusTask StatusTask{EStatusTask::NoneInit};

    // @protected Owner player controller
    UPROPERTY(BlueprintReadOnly)
    APlayerController* OwnerController{nullptr};

    // @protected Owner Task
    UPROPERTY(Replicated, BlueprintReadOnly)
    UQuestObject* OwnerQuestObject{nullptr};

    // @protected For add description. Example: counter [0/1]
    FString AddDescription{};

    // @protected for description to timer. Example: [10:28]
    FString TimerDescription{};

    // @protected Remain timer to complete task
    float RemainTimerCompleteTask{0.0f};

    // @protected complete task for timer handle
    FTimerHandle CompleteTaskTimerHandle;

    // @protected abort task for timer handle
    FTimerHandle AbortTaskTimerHandle;

#pragma endregion

#pragma region ActionBase

public:

    /**
     * @public To change status task
     **/
    void ChangeStatusTask(const EStatusTask& NewStatus);

    /**
     * @public Checking whether the tasks are completed
     **/
    bool IsTaskComplete() const { return StatusTask == EStatusTask::Complete; }

protected:

    /**
     * @protected Check on abort task
     **/
    virtual void CheckAbortTask();

    /**
     * @protected Check remain task complete
     **/
    virtual void CheckRemainTaskComplete();

#pragma endregion

#pragma region Signature

private:
    // @private Notify for paren list task
    FUpdateTaskSignature OnUpdateTask;

#pragma endregion
};
