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

#pragma region LogListTask

    /**
     * @public Write a log to list task
     **/
    void Print_LogListTask(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const;

#pragma endregion

#pragma region Default

public:

    // Constructor
    UListTaskBase();

    /**
     * @public Init list task to process task. this function override
     * @param1 APlayerController*
     * @param2 UQuestManager*
     * @return bool
     **/
    virtual bool InitListTask(APlayerController* PlayerController, UQuestManager* QuestManager);

    /**
     * @public Run list task to process task. this function override
     * @return bool
     **/
    virtual bool RunListTask();

    /**
     * @public Complete list task. this function override
     * @return bool
     **/
    virtual bool CompleteListTask();

    /**
     * @public Reset list task. this function override
     * @return bool
     **/
    virtual bool ResetListTask();

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

#if WITH_EDITOR

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

#pragma endregion

#pragma region ActionBase

public:

    /**
     * @public Get next list task
     * @return FSoftObjectPath
     **/
    virtual FSoftObjectPath GetNextListTask() const;

    /**
     * @public Destroy list task
     **/
    void DestroyListTask();

    /**
     * @public Generate info for points task
     * @return TArray<FTaskPoint>
     **/
    TArray<FTaskPoint> RequestGeneratePointsTaskForMissMap();

protected:

    /**
     * @protected Task Initialization Handler. Call function only on the server side.
     * @param1 UTaskBase*
     **/
    void ProcessTasks(UTaskBase* Task);

    /**
     * @protected Task Initialization Handler. Call function only on the server side.
     * @param1 UTaskBase*
     **/
    void NextInitTask();

    /**
     * @protected Register event update task. Call function only on the server side. This function override.
     * @param1 UTaskBase*
     **/
    virtual void RegisterUpdateTask(UTaskBase* Task);

    /**
     * @protected Find index last not complete task
     * @return int32
     **/
    int32 FindIndexLastNonCompleteTask() const;

    /**
     * @protected Find index some complete task
     * @return int32
     **/
    int32 FindIndexFirstCompleteTask() const;

    /**
     * @protected Checking whether all tasks are completed
     * @return bool
     **/
    bool IsAllTaskComplete() const;

    /**
     * @protected Checking that at least one of the main tasks has been completed
     * @return bool
     **/
    bool IsSomeTaskComplete() const;

    /**
     * @protected Change state for list task
     * @param1 EStatusListTask
     **/
    void ChangeStateListTask(EStatusListTask NewState);

private:

    /**
     * @public Reset all task
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

    /**
     * @public Get type list task visible or hidden
     * @return ETypeListTask
     **/
    const ETypeListTask& GetTypeListTask() const { return TypeListTask; }

    /**
     * @public Get run hidden list task
     * @return ERunHiddenListTask
     **/
    const ERunHiddenListTask& GetRunHiddenListTask() const { return RunHiddenListTask; }

    /**
     * @public Get action hidden list task
     * @return EActionHiddenListTask
     **/
    const EActionHiddenListTask& GetActionHiddenListTask() const { return ActionHiddenListTask; }

    /**
     * @public Get to init visible block
     * @return FSoftObjectPath
     **/
    const FSoftObjectPath& GetToInitVisibleBlock() const { return InitVisiblePathBlock; }

protected:

    // @protected Type list task
    UPROPERTY(EditAnywhere, Category = "Settings List task")
    ETypeListTask TypeListTask{ETypeListTask::Visible};

    // @protected Active to World
    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (AllowedClasses = "World"))
    FSoftObjectPath ActionWorld{};

#pragma region VisibleData
    
    // @protected Type run list task
    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (EditCondition = "TypeListTask == ETypeListTask::Visible", EditConditionHides))
    ETypeRunListTask TypeRunListTask{ETypeRunListTask::StepByStep};

    // @protected Type run list task
    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (EditCondition = "TypeRunListTask == ETypeRunListTask::TransferListTask && TypeListTask == ETypeListTask::Visible", EditConditionHides))
    TArray<FDataTransferToNextBlock> ArrayIndexTransferToNextBlocks;

    // @protected Next path block
    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (MetaClass = "ListTaskBase", EditCondition = "TypeRunListTask != ETypeRunListTask::TransferListTask && TypeListTask == ETypeListTask::Visible", EditConditionHides))
    FSoftClassPath NextPathBlock;

#pragma endregion 

#pragma region HiddenData

    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (EditCondition = "TypeListTask == ETypeListTask::Hidden", EditConditionHides))
    ERunHiddenListTask RunHiddenListTask{ERunHiddenListTask::AddToQuest};

    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (EditCondition = "TypeListTask == ETypeListTask::Hidden && RunHiddenListTask == ERunHiddenListTask::InitToRunListTask", EditConditionHides))
    FSoftClassPath InitVisiblePathBlock;
    
    UPROPERTY(EditAnywhere, Category = "Settings List task", meta = (EditCondition = "TypeListTask == ETypeListTask::Hidden", EditConditionHides))
    EActionHiddenListTask ActionHiddenListTask{EActionHiddenListTask::Nothing};

#pragma endregion
    
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

public:
    
    FUpdateListTaskSignature OnUpdateListTask;

#pragma endregion
    
};
