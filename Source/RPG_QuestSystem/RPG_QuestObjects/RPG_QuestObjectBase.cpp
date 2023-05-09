/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestObjectBase.h"
#include "JsonObjectConverter.h"
#include "Net/UnrealNetwork.h"
#include "RPG_Components/RPG_QuestManagerBase.h"
#include "RPG_Librarys/RPG_QuestSystemLibrary.h"
#include "RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_QuestSystem/RPG_Config/RPG_QuestSystemSettings.h"

#pragma region Log

void URPG_QuestObjectBase::Print_LogQuest(const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const
{
    const FString NetMode = URPG_QuestSystemLibrary::GetNetModeToString(OwnerPC);
    const FString Result = FString::Printf(TEXT("NetMode: [%s] | Name quest object: [%s] | Text: [%s]"), *NetMode, *GetName(), *Text);
    URPG_QuestSystemLibrary::Print_Log(LogVerb, Result, Line, Function);
}

bool URPG_QuestObjectBase::Print_CLogQuest(const bool bCond, const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const
{
    const FString NetMode = URPG_QuestSystemLibrary::GetNetModeToString(OwnerPC);
    const FString Result = FString::Printf(TEXT("NetMode: [%s] | Name quest object: [%s] | Text: [%s]"), *NetMode, *GetName(), *Text);
    return URPG_QuestSystemLibrary::Print_CLog(bCond, LogVerb, Result, Line, Function);
}

#pragma endregion

#pragma region DefaultInterface

URPG_QuestObjectBase::URPG_QuestObjectBase()
{
    if (HasAnyFlags(RF_ClassDefaultObject)) return;

#if UE_EDITOR

    if (!FindStartTaskNode())
    {
        RemoveAllTaskNode();
        CreateNewStartTaskNode();
    }

#endif
}

bool URPG_QuestObjectBase::InitQuest(APlayerController* PlayerController, URPG_QuestManagerBase* ParentQuestManager)
{
    if (QUEST_OBJECT_CLOG(StateQuestObject != ERPG_StateEntity::None, Error, FString::Printf(TEXT("Quest is not state NONE | Quest current state: [%s]"), *UEnum::GetValueAsString(StateQuestObject))))
        return false;

    if (QUEST_OBJECT_CLOG(!IsValid(PlayerController), Error, TEXT("Player Controller is nullptr"))) return false;
    if (QUEST_OBJECT_CLOG(!IsValid(ParentQuestManager), Error, TEXT("Parent Quest manager is nullptr"))) return false;

    OwnerQM = ParentQuestManager;
    OwnerPC = PlayerController;

    ChangeStateQuestObject(ERPG_StateEntity::Init);
    return true;
}

void URPG_QuestObjectBase::RunQuest()
{
    if (QUEST_OBJECT_CLOG(StateQuestObject != ERPG_StateEntity::Init, Error, FString::Printf(TEXT("Quest is not state INIT | Quest current state: [%s]"), *UEnum::GetValueAsString(StateQuestObject))))
        return;

    const FRPG_TaskNodeData* StartNode = FindStartTaskNode();
    if (QUEST_OBJECT_CLOG(!StartNode, Error, TEXT("StartNode is not found"))) return;

    ChangeStateQuestObject(ERPG_StateEntity::Run);

    for (int32 IndexNode : StartNode->OutNodes)
    {
        const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::StartTask, IndexNode);
        GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
    }
}

void URPG_QuestObjectBase::CompleteQuest()
{
    if (QUEST_OBJECT_CLOG(StateQuestObject != ERPG_StateEntity::Run, Error, FString::Printf(TEXT("Quest is not state INIT | Quest current state: [%s]"), *UEnum::GetValueAsString(StateQuestObject))))
        return;

    for (const int32 IndexNode : ArrayActiveIndexTask)
    {
        FTimerDelegate StopTaskTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::StopTask, IndexNode, true);
        GetWorld()->GetTimerManager().SetTimerForNextTick(StopTaskTimerDelegate);
    }
    const FTimerDelegate ChangeStateTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::ChangeStateQuestObject, ERPG_StateEntity::Complete);
    GetWorld()->GetTimerManager().SetTimerForNextTick(ChangeStateTimerDelegate);
}

void URPG_QuestObjectBase::ResetQuest()
{
    TArray<URPG_TaskNodeBase*> TaskNodes = GetArrayInstanceTaskNodes();
    for (URPG_TaskNodeBase* Task : TaskNodes)
    {
        if (!Task) continue;
        if (Task->GetStateTaskNode() != ERPG_StateEntity::Complete) continue;
        Task->ResetTask();
    }
    ChangeStateQuestObject(ERPG_StateEntity::None);
}

void URPG_QuestObjectBase::Serialize(FArchive& Ar)
{
    UObject::Serialize(Ar);

    if (Ar.IsSaving())
    {
        QUEST_OBJECT_LOG(Display, FString::Printf(TEXT("----| SERIALIZE SAVE QUEST: [%s] |----"), *GetName()));
    }
    else
    {
        QUEST_OBJECT_LOG(Display, FString::Printf(TEXT("----| SERIALIZE LOAD QUEST: [%s] |----"), *GetName()));
    }
    PrintTaskNodes();
    QUEST_OBJECT_LOG(Display, FString::Printf(TEXT("----| END SERIALIZE QUEST: [%s] |----"), *GetName()));
}

#pragma endregion

#pragma region NetworkInterface

int32 URPG_QuestObjectBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    if (!OwnerPC && GetWorld()->GetNetMode() == NM_Client)
    {
        OwnerPC = GetWorld()->GetFirstPlayerController();
    }
    return (OwnerPC ? OwnerPC->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool URPG_QuestObjectBase::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
    if (!OwnerPC && GetWorld()->GetNetMode() == NM_Client)
    {
        OwnerPC = GetWorld()->GetFirstPlayerController();
    }
    if (!OwnerPC) return false;
    UNetDriver* NetDriver = OwnerPC->GetNetDriver();
    if (!NetDriver) return false;
    NetDriver->ProcessRemoteFunction(OwnerPC, Function, Parms, OutParms, Stack, this);
    return true;
}

void URPG_QuestObjectBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(URPG_QuestObjectBase, StateQuestObject, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(URPG_QuestObjectBase, ArrayTaskNodes, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(URPG_QuestObjectBase, ArrayActiveIndexTask, COND_OwnerOnly);
}

#pragma endregion

#pragma region ActionQuestObject

void URPG_QuestObjectBase::ChangeStateQuestObject(ERPG_StateEntity NewState)
{
    if (QUEST_OBJECT_CLOG(StateQuestObject == NewState, Warning, FString::Printf(TEXT("StateQuestObject equal New state: [%s]"), *UEnum::GetValueAsString(StateQuestObject)))) return;

    StateQuestObject = NewState;
    QUEST_OBJECT_LOG(Display, FString::Printf(TEXT("StateQuestObject changed to: [%s]"), *UEnum::GetValueAsString(StateQuestObject)));
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::NotifyUpdateStateQuest);
}

void URPG_QuestObjectBase::StartTask(int32 Index)
{
    if (QUEST_OBJECT_CLOG(StateQuestObject != ERPG_StateEntity::Run, Error, TEXT("StateQuestObject is not Running"))) return;

    FRPG_TaskNodeData* TaskNodeData = FindTaskNodeByElem(Index);
    if (QUEST_OBJECT_CLOG(TaskNodeData == nullptr, Display, TEXT("TaskNodeData is not found"))) return;
    if (TaskNodeData->TypeNode == ERPG_TypeNode::FinishNode)
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::CompleteQuest);
        return;
    }

    if (QUEST_OBJECT_CLOG(TaskNodeData->TaskNodeBase == nullptr, Display, TEXT("TaskNodeBase is nullptr"))) return;
    TaskNodeData->TaskNodeBase->ResetTask();
    if (TaskNodeData->TaskNodeBase->InitTask(OwnerPC, this))
    {
        ArrayActiveIndexTask.AddUnique(Index);
        TaskNodeData->TaskNodeBase->OnUpdateTaskNode.BindUObject(this, &ThisClass::RegisterUpdateStateTask_Event);
        TaskNodeData->TaskNodeBase->RunTask();
    }
    else
    {
        StopTask(Index);
        for (int32 IndexNode : TaskNodeData->OutNodes)
        {
            const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::StartTask, IndexNode);
            GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
        }
    }
}

void URPG_QuestObjectBase::StopTask(int32 Index, bool WithResetTask)
{
    if (QUEST_OBJECT_CLOG(StateQuestObject != ERPG_StateEntity::Run, Error, TEXT("StateQuestObject is not Running"))) return;

    FRPG_TaskNodeData* TaskNodeData = FindTaskNodeByElem(Index);
    if (QUEST_OBJECT_CLOG(TaskNodeData == nullptr, Display, TEXT("TaskNodeData is not found"))) return;
    if (QUEST_OBJECT_CLOG(TaskNodeData->TaskNodeBase == nullptr, Display, TEXT("TaskNodeBase is nullptr"))) return;

    TaskNodeData->TaskNodeBase->OnUpdateTaskNode.Unbind();
    if (WithResetTask)
    {
        TaskNodeData->TaskNodeBase->ResetTask();
    }
    ArrayActiveIndexTask.Remove(TaskNodeData->IndexNode);
}

void URPG_QuestObjectBase::RegisterUpdateStateTask_Event(URPG_TaskNodeBase* TaskNode)
{
    if (QUEST_OBJECT_CLOG(TaskNode == nullptr, Error, TEXT("TaskNode is nullptr"))) return;

    if (TaskNode->GetStateTaskNode() == ERPG_StateEntity::Complete)
    {
        FRPG_TaskNodeData* TaskNodeData = FindTaskNodeByTask(TaskNode);
        if (!TaskNodeData) return;
        StopTask(TaskNodeData->IndexNode);
        for (int32 IndexNode : TaskNodeData->OutNodes)
        {
            const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::StartTask, IndexNode);
            GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
        }
    }
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::NotifyUpdateStateQuest);
}

TArray<URPG_TaskNodeBase*> URPG_QuestObjectBase::GetArrayInstanceTaskNodes()
{
    TArray<URPG_TaskNodeBase*> Results;

    for (const auto& Data : ArrayTaskNodes)
    {
        if (!Data.TaskNodeBase) continue;
        Results.Append(Data.TaskNodeBase->GetAllInstanced());
    }
    return Results;
}

TArray<URPG_TaskNodeBase*> URPG_QuestObjectBase::GetArrayActiveTaskNodes()
{
    TArray<URPG_TaskNodeBase*> Results;

    for (int32 IndexNode : ArrayActiveIndexTask)
    {
        FRPG_TaskNodeData* Node = FindTaskNodeByElem(IndexNode);
        if (!Node) continue;
        if (!Node->TaskNodeBase) continue;
        Results.Add(Node->TaskNodeBase);
    }
    return Results;
}

FRPG_TaskNodeData* URPG_QuestObjectBase::FindTaskNodeByElem(int32 IndexNode)
{
    return ArrayTaskNodes.FindByPredicate([IndexNode](const FRPG_TaskNodeData& Data) { return Data.IndexNode == IndexNode; });
}

FRPG_TaskNodeData* URPG_QuestObjectBase::FindTaskNodeByTask(URPG_TaskNodeBase* TaskNode)
{
    return ArrayTaskNodes.FindByPredicate([TaskNode](const FRPG_TaskNodeData& Data) { return Data.TaskNodeBase == TaskNode; });
}

FRPG_TaskNodeData* URPG_QuestObjectBase::FindStartTaskNode()
{
    return ArrayTaskNodes.FindByPredicate([](const FRPG_TaskNodeData& Data) { return Data.TypeNode == ERPG_TypeNode::StartNode; });
}

int32 URPG_QuestObjectBase::FindTaskNodeByIndex(int32 IndexNode) const
{
    return ArrayTaskNodes.IndexOfByPredicate([IndexNode](const FRPG_TaskNodeData& Data) { return Data.IndexNode == IndexNode; });
}

int32 URPG_QuestObjectBase::FindStartTaskNodeByIndex() const
{
    return ArrayTaskNodes.IndexOfByPredicate([](const FRPG_TaskNodeData& Data) { return Data.TypeNode == ERPG_TypeNode::StartNode; });
}

#if UE_EDITOR

FRPG_TaskNodeData* URPG_QuestObjectBase::CreateNewTaskNode(UClass* ClassNode, const FVector2D& Location, const ERPG_TypeNode& TypeNode)
{
    const int32 FreeSlot = IsFreeIndexNumSlot();
    if (FreeSlot == INDEX_NONE) return nullptr;

    FRPG_TaskNodeData NewTaskNode;
    NewTaskNode.TypeNode = TypeNode;
    NewTaskNode.IndexNode = FreeSlot;
    NewTaskNode.NodePosition = Location;
    if (ClassNode)
    {
        NewTaskNode.TaskNodeBase = NewObject<URPG_TaskNodeBase>(this, ClassNode, NAME_None, RF_Transactional);
        if (!NewTaskNode.TaskNodeBase) return nullptr;
    }

    ArrayTaskNodes.Add(NewTaskNode);
    PostEditChange();
    MarkPackageDirty();
    return FindTaskNodeByElem(FreeSlot);
}

FRPG_TaskNodeData* URPG_QuestObjectBase::CreateNewStartTaskNode()
{
    const int32 FreeSlot = IsFreeIndexNumSlot();
    if (FreeSlot == INDEX_NONE) return nullptr;

    FRPG_TaskNodeData NewTaskNode;
    NewTaskNode.TypeNode = ERPG_TypeNode::StartNode;
    NewTaskNode.IndexNode = FreeSlot;

    ArrayTaskNodes.AddUnique(NewTaskNode);
    PostEditChange();
    MarkPackageDirty();
    return FindStartTaskNode();
}

void URPG_QuestObjectBase::RemoveTaskNode(int32 IndexNode)
{
    const int32 LocalArrayIndex = FindTaskNodeByIndex(IndexNode);
    if (!ArrayTaskNodes.IsValidIndex(LocalArrayIndex)) return;
    if (ArrayTaskNodes[LocalArrayIndex].TaskNodeBase)
    {
        ArrayTaskNodes[LocalArrayIndex].TaskNodeBase->MarkAsGarbage();
        ArrayTaskNodes[LocalArrayIndex].TaskNodeBase = nullptr;
    }
    ArrayTaskNodes.RemoveAt(LocalArrayIndex);
}

void URPG_QuestObjectBase::RemoveAllTaskNode()
{
    for (auto& Data : ArrayTaskNodes)
    {
        Data.ClearData();
    }
    ArrayTaskNodes.Empty();
}

FLinearColor URPG_QuestObjectBase::GetNodeTitleColor(int32 TargetIndexNode) const
{
    const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>();
    if (!QuestSystemSettings) return FLinearColor();
    int32 IndexTaskNodeData = FindTaskNodeByIndex(TargetIndexNode);
    if (!ArrayTaskNodes.IsValidIndex(IndexTaskNodeData)) return FLinearColor();

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::StartNode)
    {
        return QuestSystemSettings->StartTaskNodeEditorColor.NodeTitleColor;
    }

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::StandardNode)
    {
        return QuestSystemSettings->StandardTaskNodeEditorColor.NodeTitleColor;
    }

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::FinishNode)
    {
        return QuestSystemSettings->FinishTaskNodeEditorColor.NodeTitleColor;
    }

    return FLinearColor();
}

FLinearColor URPG_QuestObjectBase::GetNodeCommentColor(int32 TargetIndexNode) const
{
    const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>();
    if (!QuestSystemSettings) return FLinearColor();
    int32 IndexTaskNodeData = FindTaskNodeByIndex(TargetIndexNode);
    if (!ArrayTaskNodes.IsValidIndex(IndexTaskNodeData)) return FLinearColor();

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::StartNode)
    {
        return QuestSystemSettings->StartTaskNodeEditorColor.NodeCommentColor;
    }

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::StandardNode)
    {
        return QuestSystemSettings->StandardTaskNodeEditorColor.NodeCommentColor;
    }

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::FinishNode)
    {
        return QuestSystemSettings->FinishTaskNodeEditorColor.NodeCommentColor;
    }

    return FLinearColor();
}

FLinearColor URPG_QuestObjectBase::GetNodeBodyTintColor(int32 TargetIndexNode) const
{
    const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>();
    if (!QuestSystemSettings) return FLinearColor();
    int32 IndexTaskNodeData = FindTaskNodeByIndex(TargetIndexNode);
    if (!ArrayTaskNodes.IsValidIndex(IndexTaskNodeData)) return FLinearColor();

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::StartNode)
    {
        return QuestSystemSettings->StartTaskNodeEditorColor.NodeBodyTintColor;
    }

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::StandardNode)
    {
        return QuestSystemSettings->StandardTaskNodeEditorColor.NodeBodyTintColor;
    }

    if (ArrayTaskNodes[IndexTaskNodeData].TypeNode == ERPG_TypeNode::FinishNode)
    {
        return QuestSystemSettings->FinishTaskNodeEditorColor.NodeBodyTintColor;
    }

    return FLinearColor();
}

FString URPG_QuestObjectBase::GetCategoryNameByTypeNode(const ERPG_TypeNode& TypeNode) const
{
    const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>();

    if (QuestSystemSettings && QuestSystemSettings->NameCategoryByTypeNode.Contains(TypeNode))
    {
        return QuestSystemSettings->NameCategoryByTypeNode[TypeNode];
    }
    return TEXT("None");
}

int32 URPG_QuestObjectBase::IsFreeIndexNumSlot() const
{
    for (int32 i = 0; i < INT32_MAX; i++)
    {
        if (!ArrayTaskNodes.ContainsByPredicate([i](const FRPG_TaskNodeData& Data) { return Data.IndexNode == i; }))
        {
            return i;
        }
    }
    return INDEX_NONE;
}

void URPG_QuestObjectBase::PrintTaskNodes()
{
    QUEST_OBJECT_LOG(Display, FString::Printf(TEXT("----| START PRINT JSON TASK NODES FOR QUEST: [%s] |----"), *GetName()));
    float SizeArrayTaskNodesJSON = 0.0f;
    for (auto& Data : ArrayTaskNodes)
    {
        FString JSONData;
        FJsonObjectConverter::UStructToJsonObjectString(Data, JSONData);
        QUEST_OBJECT_LOG(Display, JSONData);
        SizeArrayTaskNodesJSON += (JSONData.Len() + 1) / 1024.0f;
    }
    QUEST_OBJECT_LOG(Display, FString::Printf(TEXT("Size json data: [%.2f] KB"), SizeArrayTaskNodesJSON));

    SIZE_T GeneralSize = 0;
    for (const auto& Data : ArrayTaskNodes)
    {
        if (!Data.TaskNodeBase) continue;
        TArray<URPG_TaskNodeBase*> ArrayTask = Data.TaskNodeBase->GetAllInstanced();
        for (URPG_TaskNodeBase* Node : ArrayTask)
        {
            GeneralSize += Node->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal);
        }
    }

    const float SizeArrayTaskNodes = static_cast<float>(GeneralSize / 1024.f);
    QUEST_OBJECT_LOG(Display, FString::Printf(TEXT("All Task size: [%.2f] KB"), SizeArrayTaskNodes));
    QUEST_OBJECT_LOG(Display, FString::Printf(TEXT("----| END PRINT JSON TASK NODES FOR QUEST: [%s] |----"), *GetName()));
}

#endif

#pragma endregion
