/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_TaskNodes/RPG_ExampleTaskNodes/RPG_DelayTaskNode.h"

#pragma region DefaultInterfaceTaskNodeBase

URPG_DelayTaskNode::URPG_DelayTaskNode() {}

bool URPG_DelayTaskNode::InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest)
{
    return Super::InitTask(PlayerController, ParentQuest);
}

void URPG_DelayTaskNode::RunTask()
{
    if (GetWorld())
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::CompleteTask, DelayTime, false);
    }
    return Super::RunTask();
}

void URPG_DelayTaskNode::CompleteTask()
{
    Super::CompleteTask();
}

void URPG_DelayTaskNode::ResetTask()
{
    Super::ResetTask();
}

#pragma endregion
