// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestManagerBase.h"

#include "Engine/ActorChannel.h"
#include "ListTask/VisibleListTask.h"
#include "Net/UnrealNetwork.h"

#pragma region LogQuestManager

void UQuestManagerBase::Print_LogQuestManager(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const
{
    if (!GetOwner()) return;
    const FString StrNetMode = UQuestLibrary::GetEnumValueAsString("ENetMode", GetOwner()->GetNetMode());
    UQuestLibrary::Print_Log(LogVerb, FString::Printf(TEXT("NetMode: [%s] | Owner: [%s] | Name: [%s] | %s"),
        *StrNetMode, *GetOwner()->GetName(), *GetName(), *Text), Line, Function);
}

#pragma endregion

#pragma region Default

// Sets default values for this component's properties
UQuestManagerBase::UQuestManagerBase()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UQuestManagerBase::BeginPlay()
{
    Super::BeginPlay();

    if (CHECK_COND(GetOwner() != nullptr, "Owner is nullptr")) return;
}

// Called every frame
void UQuestManagerBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UQuestManagerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UQuestManagerBase, ArrayDataQuest, COND_OwnerOnly);
}

bool UQuestManagerBase::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    // Replicated quests and task from quests
    if (!QueuePushReplicateObject.IsEmpty())
    {
        uint32 TempUniqID;
        QueuePushReplicateObject.Dequeue(TempUniqID);
        if (const auto ListTask = FindListTaskFromID(TempUniqID))
        {
            LOG_QM(ELogVerb::Display, FString::Printf(TEXT("List Task: [%s] replicated by ID: %lu"), *ListTask->GetName(), TempUniqID));

            WroteSomething |= Channel->ReplicateSubobject(ListTask, *Bunch, *RepFlags);

            TArray<UTaskBase*> Tasks = ListTask->GetArrayTask();
            WroteSomething |= Channel->ReplicateSubobjectList(Tasks, *Bunch, *RepFlags);
        }
    }

    return WroteSomething;
}

#pragma endregion

#pragma region ActionBase

void UQuestManagerBase::PushReplicateID(const uint32 ID)
{
    if (!CHECK_COND(GetOwner()->HasAuthority(), "Function called on client ???")) return;

    QueuePushReplicateObject.Enqueue(ID);
}

#pragma endregion

#pragma region FindData

UListTaskBase* UQuestManagerBase::FindListTaskFromID(const uint32 ID) const
{
    const auto FindElem = ArrayDataQuest.FindByPredicate([ID](const FDataQuest& DataQuest)
    {
        return DataQuest.ActiveVisibleListTask->GetUniqueID() == ID;
    });

    return FindElem ? FindElem->ActiveVisibleListTask : nullptr;
}

#pragma endregion
