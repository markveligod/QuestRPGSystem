/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_QuestSystem/RPG_QuestSystemDataTypes.h"
#include "RPG_TaskNodeBase.generated.h"

class URPG_QuestObjectBase;
/**
 * @class The basic functionality of the task for the work of the quest system
 */
UCLASS(Abstract, EditInlineNew)
class RPG_QUESTSYSTEM_API URPG_TaskNodeBase : public UObject
{
    GENERATED_BODY()

    friend class URPG_QuestObjectBase;

#pragma region Log

protected:
#define TASK_NODE_LOG(LogVerb, Text) Print_LogTask(LogVerb, Text, __LINE__, __FUNCTION__)
#define TASK_NODE_CLOG(Cond, LogVerb, Text) Print_CLogTask(Cond, LogVerb, Text, __LINE__, __FUNCTION__)

    /** @protected Write a log **/
    void Print_LogTask(const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const;

    /** @protected Write a clog **/
    bool Print_CLogTask(const bool bCond, const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const;

#pragma endregion

#pragma region DefaultInterface

public:
    URPG_TaskNodeBase();

    /** @public Return a one line description of an object for viewing in the thumbnail view of the generic browser */
    virtual FString GetDesc() override { return GetClass() ? GetClass()->GetName() : GetName(); }

protected:

    /** @protected Initializing a task. Call only Server side **/
    virtual bool InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest);

    /** @protected Run a task. Call only Server side  **/
    virtual void RunTask();

    /** @protected Complete a task. Call only Server side  **/
    virtual void CompleteTask();

    /** @protected Reset a task. Call only Server side  **/
    virtual void ResetTask();

protected:

    /** @protected Initializing a task to blueprint event. Call only Server side **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    bool InitTask_Event();

    /** @protected Run a task to blueprint event. Call only Server side **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    void RunTask_Event();

    /** @protected Complete a task to blueprint event. Call only Server side **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    void CompleteTask_Event();

    /** @protected Reset a task to blueprint event. Call only Server side **/
    UFUNCTION(BlueprintNativeEvent, meta = (BlueprintProtected = true))
    void ResetTask_Event();

public:

    /** @public Get all instance tasks**/
    UFUNCTION(BlueprintCallable)
    virtual TArray<URPG_TaskNodeBase*> GetAllInstanced();

#pragma endregion

#pragma region NetworkInterface

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

#pragma region ActionTaskNode

public:

    /** @public Getting a description of the current task **/
    UFUNCTION(BlueprintCallable)
    const FText& GetDescriptionTask() { return DescriptionTask; }

    /** @public Getting the current state of the task entity **/
    UFUNCTION(BlueprintCallable)
    const ERPG_StateEntity& GetStateTaskNode() { return StateTaskNode; }

private:

    /** @private Changing the state of the task entity **/
    UFUNCTION(BlueprintCallable, meta = (BlueprintProtected = true))
    void ChangeStateTaskNode(ERPG_StateEntity NewState);

#pragma endregion

#pragma region DataTask

protected:

    /** @protected Description of the current task **/
    UPROPERTY(EditAnywhere, meta = (MultiLine = true))
    FText DescriptionTask;

    /** @protected The controller that owns this task **/
    UPROPERTY(BlueprintReadOnly)
    APlayerController* OwnerPC{nullptr};

    /** @protected The quest that owns the current task **/
    UPROPERTY(BlueprintReadOnly)
    URPG_QuestObjectBase* OwnerQuestObject{nullptr};

private:
    
    /** @private Current status of the node task **/
    ERPG_StateEntity StateTaskNode{ERPG_StateEntity::None};

#pragma endregion

#pragma region Signatures

    friend class URPG_InstanceTaskNode;

protected:
    
    /** @protected Notification if the task has been updated data or state **/
    UFUNCTION(BlueprintCallable, meta = (BlueprintProtected = true))
    void NotifyUpdateTaskNode() { OnUpdateTaskNode.ExecuteIfBound(this); }

    DECLARE_DELEGATE_OneParam(FUpdateTaskNodeSignature, class URPG_TaskNodeBase*);

    /** @protected Update signature task node **/
    FUpdateTaskNodeSignature OnUpdateTaskNode;

#pragma endregion
};
