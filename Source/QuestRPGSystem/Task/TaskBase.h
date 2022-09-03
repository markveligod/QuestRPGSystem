// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskDataTypes.h"
#include "Librarys/QuestLibrary.h"
#include "TaskBase.generated.h"

class UListTaskBase;

#define LOG_TASK(LogVerb, Text) Print_LogTask(LogVerb, Text, __LINE__, __FUNCTION__)

/**
 * @class Task base
 */
UCLASS()
class QUESTRPGSYSTEM_API UTaskBase : public UObject
{
    GENERATED_BODY()

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

#pragma endregion

#pragma region DataTask

protected:

    // @protected Task description
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Task", meta = (DisplayPriority = "0"))
    FText TaskDescription{};

    // @protected Notification of the quest update at the start of a current task
    UPROPERTY(EditDefaultsOnly, Category = "Settings Task", meta = (DisplayPriority = 1))
    bool bIsLandMarkUpdate{true};

    // @protected Task specific settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings Task")
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

#pragma region Action

public:

    /**
     * @public To change status task
     * @param1 EStatusTask
     **/
    void ChangeStatusTask(const EStatusTask& NewStatus);

#pragma endregion

#pragma region Signature

public:
    
    // @public Notify for paren list task
    FUpdateTaskSignature OnUpdateTask;

#pragma endregion
};
