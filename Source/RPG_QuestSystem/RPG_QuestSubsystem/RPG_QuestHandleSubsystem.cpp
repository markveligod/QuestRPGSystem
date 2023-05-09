/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestSubsystem/RPG_QuestHandleSubsystem.h"

#pragma region DefaultInterface

void URPG_QuestHandleSubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

bool URPG_QuestHandleSubsystem::IsTickable() const
{
    return true;
}

TStatId URPG_QuestHandleSubsystem::GetStatId() const
{
    return TStatId();
}

void URPG_QuestHandleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void URPG_QuestHandleSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

#pragma endregion
