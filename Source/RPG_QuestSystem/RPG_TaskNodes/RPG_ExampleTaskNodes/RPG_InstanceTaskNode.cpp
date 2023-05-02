/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_TaskNodes/RPG_ExampleTaskNodes/RPG_InstanceTaskNode.h"

#pragma region DefaultInterfaceTaskNodeBase

URPG_InstanceTaskNode::URPG_InstanceTaskNode() {}

bool URPG_InstanceTaskNode::InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest)
{
    if (TASK_NODE_CLOG(ExecuteTasks.Num() == 0, Error, FString("ExecuteTasks is empty"))) return false;

    return Super::InitTask(PlayerController, ParentQuest);
}

void URPG_InstanceTaskNode::RunTask()
{
    for (auto* Task : ExecuteTasks)
    {
        if (!Task) continue;
        if (Task->InitTask(OwnerPC, OwnerQuestObject))
        {
            Task->OnUpdateTaskNode.BindUObject(this, &ThisClass::RegisterUpdateTask);
            Task->RunTask();
        }
    }
    return Super::RunTask();
}

void URPG_InstanceTaskNode::CompleteTask()
{
    for (auto* Task : ExecuteTasks)
    {
        if (!Task) continue;
        Task->OnUpdateTaskNode.Unbind();
    }
    Super::CompleteTask();
}

void URPG_InstanceTaskNode::ResetTask()
{
    for (auto* Task : ExecuteTasks)
    {
        if (!Task) continue;
        Task->OnUpdateTaskNode.Unbind();
        Task->ResetTask();
    }
    Super::ResetTask();
}

TArray<URPG_TaskNodeBase*> URPG_InstanceTaskNode::GetAllInstanced()
{
    TArray<URPG_TaskNodeBase*> AllInst = Super::GetAllInstanced();
    for (auto* Task : ExecuteTasks)
    {
        if (!Task) continue;
        AllInst.Append(Task->GetAllInstanced());
    }

    return AllInst;
}

#pragma endregion

#pragma region ActionTask

void URPG_InstanceTaskNode::RegisterUpdateTask(URPG_TaskNodeBase* Task)
{
    if (!Task) return;

    const bool bIsTaskInRun = ExecuteTasks.ContainsByPredicate([](URPG_TaskNodeBase* Task) { return Task ? Task->GetStateTaskNode() == ERPG_StateEntity::Run : false; });

    if (!bIsTaskInRun)
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::CompleteTask);
    }
}

#pragma endregion
