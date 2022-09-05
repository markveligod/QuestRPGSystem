// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListTaskDataTypes.h"
#include "Engine/DataAsset.h"
#include "Librarys/QuestLibrary.h"
#include "ListTaskBase.generated.h"

class UQuestManager;
class UTaskBase;

#define LOG_LIST_TASK(LogVerb, Text) Print_LogListTask(LogVerb, Text, __LINE__, __FUNCTION__)

/**
 * @class Base list task data asset
 */
UCLASS(Abstract)
class QUESTRPGSYSTEM_API UListTaskBase : public UDataAsset
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
     * @public Return the space this function should be called.   Checks to see if this function should
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



#pragma endregion

#pragma region DataListTask

public:

    /**
     * @public  Getting constant data array task
     * @return TArray<UTaskBase*>
     **/
    UFUNCTION(BlueprintCallable, Category = "DataListTask")
    const TArray<UTaskBase*>& GetArrayTask() { return ArrayTask; }

    /**
     * @public  Getting status list task
     * @return EStatusListTask
     **/
    UFUNCTION(BlueprintCallable, Category = "DataListTask")
    const EStatusListTask& GetStatusListTask() const { return StatusListTask; }

    /**
     * @public  Getting type run list task
     * @return ETypeRunListTask
     **/
    UFUNCTION(BlueprintCallable, Category = "DataListTask")
    const ETypeRunListTask& GetTypeRunListTask() const { return TypeRunListTask; }

protected:

    // @protected Type run list task
    UPROPERTY(Replicated, EditAnywhere, Category = "Settings List task")
    ETypeRunListTask TypeRunListTask{ETypeRunListTask::StepByStep};

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
    
    FUpdateListTaskSignature OnUpdateListTask;

#pragma endregion
    
};
