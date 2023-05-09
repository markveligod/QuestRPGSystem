// Fill out your copyright notice in the Description page of Project Settings.


#include "RPG_TaskNodes/RPG_ExampleTaskNodes/RPG_BroadcastTaskNode.h"
#include "RPG_QuestSubsystem/RPG_QuestHandleSubsystem.h"

#pragma region DefaultInterfaceTaskNodeBase

URPG_BroadcastTaskNode::URPG_BroadcastTaskNode()
{
}

bool URPG_BroadcastTaskNode::InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest)
{
    if (TASK_NODE_CLOG(!Tag.IsValid(), Error, FString("Tag is not valid"))) return false;

    return Super::InitTask(PlayerController, ParentQuest);
}

void URPG_BroadcastTaskNode::RunTask()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::ActionBroadcast);
    }
    Super::RunTask();
}

void URPG_BroadcastTaskNode::CompleteTask()
{
    Super::CompleteTask();
}

void URPG_BroadcastTaskNode::ResetTask()
{
    Super::ResetTask();
}

void URPG_BroadcastTaskNode::ActionBroadcast()
{
    if (URPG_QuestHandleSubsystem* QuestHandleSubsystem = GetWorld()->GetSubsystem<URPG_QuestHandleSubsystem>())
    {
        QuestHandleSubsystem->ActionBroadcastQuestTag(OwnerPC, Tag);
    }
}

#pragma endregion

