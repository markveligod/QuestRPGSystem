/** Copyright Mark Veligod. Published in 2023. **/


#include "RPG_TaskNodes/RPG_ExampleTaskNodes/RPG_MoveToTaskNode.h"

#pragma region DefaultInterfaceTaskNodeBase

URPG_MoveToTaskNode::URPG_MoveToTaskNode()
{
}

bool URPG_MoveToTaskNode::InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest)
{
    if (TASK_NODE_CLOG(TargetPosition == FVector::ZeroVector, Error, FString("TargetPosition is Zero vector"))) return false;

    return Super::InitTask(PlayerController, ParentQuest);
}

void URPG_MoveToTaskNode::RunTask()
{
    if (UWorld* World = GetWorld())
    {
        TASK_NODE_LOG(Display, TEXT("Start check distance to position"));
        World->GetTimerManager().SetTimer(CheckDistToPosTimerHandle,this, &ThisClass::RegisterCheckDistToPos, FrequencyCheckDist, true);
    }
    return Super::RunTask();
}

void URPG_MoveToTaskNode::CompleteTask()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(CheckDistToPosTimerHandle);
    }
    return Super::CompleteTask();
}

void URPG_MoveToTaskNode::ResetTask()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(CheckDistToPosTimerHandle);
    }
    Super::ResetTask();
}

#pragma endregion

#pragma region ActionTask

void URPG_MoveToTaskNode::RegisterCheckDistToPos()
{
    if (OwnerPC && OwnerPC->GetPawn())
    {
        const float CalcDist = FVector::Dist(OwnerPC->GetPawn()->GetActorLocation(), TargetPosition);
        if (CalcDist <= RadiusComplete)
        {
            TASK_NODE_LOG(Display, FString::Printf(TEXT("Radius complete done | Target Radius: [%f] - CalcDist: [%f]"), RadiusComplete, CalcDist));
            CompleteTask();
        }
    }
}

#pragma endregion
