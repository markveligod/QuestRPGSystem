/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_QuestSystem/RPG_QuestSystemDataTypes.h"
#include "RPG_QuestSystem/RPG_QuestObjects/RPG_QuestObjectDataTypes.h"
#include "RPG_QuestObjectBase.generated.h"

class URPG_QuestManagerBase;
/**
 * @class The quest object that manages the tasks
 */
UCLASS()
class RPG_QUESTSYSTEM_API URPG_QuestObjectBase : public UObject
{
    GENERATED_BODY()

    friend class URPG_QuestManagerBase;

#pragma region Log

protected:
#define QUEST_OBJECT_LOG(LogVerb, Text) Print_LogQuest(LogVerb, Text, __LINE__, __FUNCTION__)
#define QUEST_OBJECT_CLOG(Cond, LogVerb, Text) Print_CLogQuest(Cond, LogVerb, Text, __LINE__, __FUNCTION__)

    /** @protected Write a log **/
    void Print_LogQuest(const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const;

    /** @protected Write a clog **/
    bool Print_CLogQuest(const bool bCond, const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const;

#pragma endregion

#pragma region DefaultInterface

public:
    URPG_QuestObjectBase();

protected:
    /** @protected Initializing a quest. Call only Server **/
    virtual bool InitQuest(APlayerController* PlayerController, URPG_QuestManagerBase* ParentQuestManager);

    /** @protected Run a quest. Call only Server **/
    virtual void RunQuest();

    /** @protected Complete a quest. Call only Server **/
    virtual void CompleteQuest();

    /** @protected Reset a quest. Call only Server **/
    virtual void ResetQuest();

    /**
     * Handles reading, writing, and reference collecting using FArchive.
     * This implementation handles all FProperty serialization, but can be overridden for native variables.
     */
    virtual void Serialize(FArchive& Ar) override;

#pragma endregion

#pragma region NetworkInterface

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

#pragma region ActionQuestObject

public:
    /** @public Getting the state of the quest entity **/
    const ERPG_StateEntity& GetStateQuest() const { return StateQuestObject; }

    /** @public Getting all task nodes **/
    const TArray<FRPG_TaskNodeData>& GetArrayTaskNodes() { return ArrayTaskNodes; }

    /** @public Getting all instance task nodes **/
    TArray<URPG_TaskNodeBase*> GetArrayInstanceTaskNodes();

    /** @public Getting all Active task nodes **/
    TArray<URPG_TaskNodeBase*> GetArrayActiveTaskNodes();

    /** @public Finding a task node element by index **/
    FRPG_TaskNodeData* FindTaskNodeByElem(int32 IndexNode);

    /** @public Finding a task node element by Task **/
    FRPG_TaskNodeData* FindTaskNodeByTask(URPG_TaskNodeBase* TaskNode);

    /** @public Search for the starting task node **/
    FRPG_TaskNodeData* FindStartTaskNode();

    /** @public Getting an index in the node array by the index of the node itself **/
    int32 FindTaskNodeByIndex(int32 IndexNode) const;

    /** @public Getting an index in the array of the starting node **/
    int32 FindStartTaskNodeByIndex() const;

#if UE_EDITOR

public:
    /** @public Creating a new working task node **/
    FRPG_TaskNodeData* CreateNewTaskNode(UClass* ClassNode, const FVector2D& Location, const ERPG_TypeNode& TypeNode);

    /** @public Creating a new start task node **/
    FRPG_TaskNodeData* CreateNewStartTaskNode();

    /** @public Removing a task node by index **/
    void RemoveTaskNode(int32 IndexNode);

    /** @public Cleaning all nodes **/
    void RemoveAllTaskNode();

    /** @public Getting node title color **/
    FLinearColor GetNodeTitleColor(int32 TargetIndexNode) const;

    /** @public Getting node comment color **/
    FLinearColor GetNodeCommentColor(int32 TargetIndexNode) const;

    /** @public Getting node body tint color **/
    FLinearColor GetNodeBodyTintColor(int32 TargetIndexNode) const;

    /** @public Getting category name by type node **/
    FString GetCategoryNameByTypeNode(const ERPG_TypeNode& TypeNode) const;

private:
    /** @private Search for a free number index **/
    int32 IsFreeIndexNumSlot() const;

    /** @private Print all nodes in JSON format to logs **/
    void PrintTaskNodes();

#endif

protected:
    /** @protected Changes to the state of the quest object entity **/
    virtual void ChangeStateQuestObject(ERPG_StateEntity NewState);

    /** @protected Start task **/
    virtual void StartTask(int32 Index);

    /** @protected Stop task **/
    virtual void StopTask(int32 Index, bool WithResetTask = false);

    /** @protected Registration receiving notification from task update event**/
    virtual void RegisterUpdateStateTask_Event(URPG_TaskNodeBase* TaskNode);

#pragma endregion

#pragma region DataQuest

protected:
    /** @protected The state of the entity according to the work of the class QuestObject **/
    UPROPERTY(Replicated)
    ERPG_StateEntity StateQuestObject{ERPG_StateEntity::None};

#define EMPTY_TASK_NODE_DATA FRPG_TaskNodeData()
    /** @protected A set of sequential task nodes **/
    UPROPERTY(SaveGame, Replicated)
    TArray<FRPG_TaskNodeData> ArrayTaskNodes;

    /** @protected The owner of the quest is the quest manager **/
    UPROPERTY()
    URPG_QuestManagerBase* OwnerQM{nullptr};

    /** @protected The general owner of the entire quest system **/
    UPROPERTY()
    APlayerController* OwnerPC{nullptr};

    /** @protected Target index task **/
    UPROPERTY(Replicated)
    TArray<int32> ArrayActiveIndexTask;

#pragma endregion

#pragma region Signatures

protected:
    /** @private Notification of a change in the quest status **/
    void NotifyUpdateStateQuest() { OnUpdateStateQuest.ExecuteIfBound(this); }

private:
    DECLARE_DELEGATE_OneParam(FUpdateStateQuestSignature, class URPG_QuestObjectBase*);

    /** @private Signature for updating the quest status **/
    FUpdateStateQuestSignature OnUpdateStateQuest;

#pragma endregion
};
