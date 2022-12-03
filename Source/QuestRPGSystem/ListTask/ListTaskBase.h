// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListTaskDataTypes.h"
#include "Librarys/QuestLibrary.h"
#include "ListTaskBase.generated.h"

class UQuestManager;
class UTaskBase;

#define LOG_LIST_TASK(LogVerb, Text) Print_LogListTask(LogVerb, Text, __LINE__, __FUNCTION__)

/**
 * @class Base list task data asset
 */
UCLASS(Blueprintable, Abstract)
class QUESTRPGSYSTEM_API UListTaskBase : public UObject
{
    GENERATED_BODY()

friend class UTaskBase;
friend class UQuestManager;
friend class UQuestManagerBase;

#pragma region LogListTask

protected:
    
    /**
     * @protected Write a log to list task
     **/
    virtual void Print_LogListTask(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const;

#pragma endregion

#pragma region Default

protected:

    // Constructor
    UListTaskBase();

    /**
     * @protected Init list task to process task. this function override
     **/
    virtual bool InitListTask(APlayerController* PlayerController, UQuestManager* QuestManager);

    /**
     * @protected Run list task to process task. this function override
     **/
    virtual bool RunListTask();

    /**
     * @protected Complete list task. this function override
     **/
    virtual bool CompleteListTask();

    /**
     * @protected Reset list task. this function override
     **/
    virtual bool ResetListTask();

public:
    
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

#pragma region ActionBase

public:

    /**
     * @public Get next list task
     **/
    virtual FSoftObjectPath GetNextListTask() const;

    /**
     * @public Destroy list task
     **/
    void DestroyListTask();

protected:

    /**
     * @protected Task Initialization Handler. Call function only on the server side.
     **/
    virtual void ProcessTasks(UTaskBase* Task);

    /**
     * @protected Task Initialization Handler. Call function only on the server side.
     **/
    virtual void NextInitTask();

    /**
     * @protected Register event update task. Call function only on the server side. This function override.
     **/
    virtual void RegisterUpdateTask(UTaskBase* Task);

    /**
     * @protected Find index last not complete task
     **/
    int32 FindIndexLastNonCompleteTask() const;

    /**
     * @protected Find index some complete task
     **/
    int32 FindIndexFirstCompleteTask() const;

    /**
     * @protected Checking whether all tasks are completed
     **/
    bool IsAllTaskComplete() const;

    /**
     * @protected Checking that at least one of the main tasks has been completed
     **/
    bool IsSomeTaskComplete() const;

    /**
     * @protected Change state for list task
     **/
    void ChangeStateListTask(EStatusListTask NewState);

private:

    /**
     * @private Reset all task
     **/
    void ResetAllTask();

#pragma endregion

#pragma region DataListTask

public:

    /**
     * @public  Getting constant data array task
     * @return TArray<UTaskBase*>
     **/
    const TArray<UTaskBase*>& GetArrayTask() { return ArrayTask; }

    /**
     * @public  Getting status list task
     * @return EStatusListTask
     **/
    const EStatusListTask& GetStatusListTask() const { return StatusListTask; }

    /**
     * @public  Getting type run list task
     * @return ETypeRunListTask
     **/
    const ETypeRunListTask& GetTypeRunListTask() const { return TypeRunListTask; }

    /**
     * @public Get path world action
     * @return FSoftObjectPath
     **/
    const FSoftObjectPath& GetPathActionWorld() const { return ActionWorld; }

protected:

    // @protected Active to World
    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (AllowedClasses = "World"))
    FSoftObjectPath ActionWorld{};

    // @protected Type run list task
    UPROPERTY(EditAnywhere, Category = "Settings List task")
    ETypeRunListTask TypeRunListTask{ETypeRunListTask::StepByStep};
    
    // @protected Next path block
    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (MetaClass = "ListTaskBase"))
    FSoftClassPath NextPathBlock;

    // @protected Set array task for action process
    UPROPERTY(Replicated, EditAnywhere, Instanced, Category = "Settings List task")
    TArray<UTaskBase*> ArrayTask;

    // @protected Status list task
    UPROPERTY(Replicated)
    EStatusListTask StatusListTask{EStatusListTask::None};

    // @protected Owner controller
    UPROPERTY(Replicated)
    APlayerController* OwnerController{nullptr};

    // @protected Parent quest manager
    UPROPERTY(Replicated)
    UQuestManager* ParentQuestManager{nullptr};

#pragma endregion

#pragma region Signature

private:
    
    FUpdateListTaskSignature OnUpdateListTask;

#pragma endregion

};
