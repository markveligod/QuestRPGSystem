/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_TaskNodes/RPG_ExampleTaskNodes/RPG_PrintStringTaskNode.h"

#pragma region DefaultInterfaceTaskNodeBase

URPG_PrintStringTaskNode::URPG_PrintStringTaskNode() {}

bool URPG_PrintStringTaskNode::InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest)
{
    if (TASK_NODE_CLOG(DisplayString.IsEmpty(), Error, FString("Display string is empty"))) return false;
    return Super::InitTask(PlayerController, ParentQuest);
}

void URPG_PrintStringTaskNode::RunTask()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::ActionPrintString);
    }
    return Super::RunTask();
}

void URPG_PrintStringTaskNode::CompleteTask()
{
    return Super::CompleteTask();
}

void URPG_PrintStringTaskNode::ResetTask()
{
    Super::ResetTask();
}

#pragma endregion

#pragma region ActionTask

void URPG_PrintStringTaskNode::ActionPrintString_Implementation()
{

#if UE_EDITOR

    if (bEnableToLog)
    {
        TASK_NODE_LOG(Display, DisplayString);
    }

    if (bEnableToDisplay && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, DrawDebugInfoMinimal.TimeToDisplay, DrawDebugInfoMinimal.DisplayColor, DisplayString, DrawDebugInfoMinimal.bNewerOnTop, DrawDebugInfoMinimal.TextScale);
    }

#endif

    ServerCompleteTask();
}

void URPG_PrintStringTaskNode::ServerCompleteTask_Implementation()
{
    CompleteTask();
}

#pragma endregion
