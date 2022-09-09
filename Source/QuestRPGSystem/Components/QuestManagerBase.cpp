// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestManagerBase.h"
#include "Engine/ActorChannel.h"
#include "ListTask/ListTaskBase.h"
#include "Net/UnrealNetwork.h"
#include "Json.h"

#pragma region LogQuestManager

void UQuestManagerBase::Print_LogQuestManager(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const
{
    if (!GetOwner()) return;
    UQuestLibrary::Print_Log(LogVerb, FString::Printf(TEXT("NetMode: [%i] | Owner: [%s] | Name: [%s] | %s"),
        GetOwner()->GetNetMode(), *GetOwner()->GetName(), *GetName(), *Text), Line, Function);
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

    if (!CHECK_COND(GetOwner() != nullptr, "Owner is nullptr")) return;
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

void UQuestManagerBase::PushReplicateID(const FName& QuestName)
{
    if (!CHECK_COND(GetOwner()->HasAuthority(), "Function called on client ???")) return;

    const FDataQuest& DataQuest = GetFreezeDataQuestFromName(QuestName);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return;
    if (!CHECK_COND(DataQuest.ActiveVisibleListTask != nullptr, "Active visible list task is nullptr")) return;

    QueuePushReplicateObject.Enqueue(DataQuest.ActiveVisibleListTask->GetUniqueID());
}

FString UQuestManagerBase::GetJSONFromArrayDataQuest()
{
    const TSharedPtr<FJsonObject> rootObj = MakeShareable(new FJsonObject());

    TArray<TSharedPtr<FJsonValue>> arrRootObj;
    for (const auto& DataQuest : ArrayDataQuest)
    {
        const TSharedPtr<FJsonObject> TempObjectDataQuest = MakeShareable(new FJsonObject());
        TempObjectDataQuest->SetStringField("StatusQuest", UEnum::GetValueAsString(DataQuest.StatusQuest));
        TempObjectDataQuest->SetStringField("NameQuestTable", DataQuest.NameQuestTable.ToString());
        TempObjectDataQuest->SetBoolField("bIsTargetQuest", DataQuest.bIsTargetQuest);

        // Visible list task
        TArray<TSharedPtr<FJsonValue>> arrVisibleListTaskObj;
        for (const auto& DataVisibleListTask : DataQuest.ArrayDataListTask)
        {
            const TSharedPtr<FJsonObject> TempObjectVisibleListTask = MakeShareable(new FJsonObject());
            TempObjectVisibleListTask->SetStringField("PathToVisibleListTask", DataVisibleListTask.PathToVisibleListTask.ToString());
            TempObjectVisibleListTask->SetBoolField("bListTaskComplete", DataVisibleListTask.bListTaskComplete);

            TArray<TSharedPtr<FJsonValue>> arrTaskObj;
            for (const auto& TaskInfo : DataVisibleListTask.ArrayDataTask)
            {
                const TSharedPtr<FJsonObject> TempObjectTaskInfo = MakeShareable(new FJsonObject());
                TempObjectTaskInfo->SetStringField("TaskID", TaskInfo.TaskID);
                TempObjectTaskInfo->SetStringField("TaskDescription", TaskInfo.TaskDescription.ToString());
                TempObjectTaskInfo->SetBoolField("bStatusTask", TaskInfo.bStatusTask);
                TempObjectTaskInfo->SetBoolField("bHideTaskDescription", TaskInfo.bHideTaskDescription);

                TSharedPtr<FJsonValueObject> TempValueTaskInfo = MakeShareable(new FJsonValueObject(TempObjectTaskInfo));
                arrTaskObj.Add(TempValueTaskInfo);
            }
            TempObjectVisibleListTask->SetArrayField("Array Task info", arrTaskObj);

            TSharedPtr<FJsonValueObject> TempValueDataLT = MakeShareable(new FJsonValueObject(TempObjectVisibleListTask));
            arrVisibleListTaskObj.Add(TempValueDataLT);
        }

        // Hidden list task
        TArray<TSharedPtr<FJsonValue>> arrHiddenListTaskObj;
        for (const auto& DataHiddenListTask : DataQuest.ArrayHiddenListTasks)
        {
            const TSharedPtr<FJsonObject> TempObjectHiddenListTask = MakeShareable(new FJsonObject());
            TempObjectHiddenListTask->SetStringField("PathToHiddenListTask", DataHiddenListTask.PathToHiddenListTask.ToString());
            TempObjectHiddenListTask->SetBoolField("bListTaskComplete", DataHiddenListTask.bListTaskComplete);

            TArray<TSharedPtr<FJsonValue>> arrTaskObj;
            for (const auto& TaskInfo : DataHiddenListTask.ArrayDataTask)
            {
                const TSharedPtr<FJsonObject> TempObjectTaskInfo = MakeShareable(new FJsonObject());
                TempObjectTaskInfo->SetStringField("TaskID", TaskInfo.TaskID);
                TempObjectTaskInfo->SetStringField("TaskDescription", TaskInfo.TaskDescription.ToString());
                TempObjectTaskInfo->SetBoolField("bStatusTask", TaskInfo.bStatusTask);
                TempObjectTaskInfo->SetBoolField("bHideTaskDescription", TaskInfo.bHideTaskDescription);

                TSharedPtr<FJsonValueObject> TempValueTaskInfo = MakeShareable(new FJsonValueObject(TempObjectTaskInfo));
                arrTaskObj.Add(TempValueTaskInfo);
            }
            TempObjectHiddenListTask->SetArrayField("Array Task info", arrTaskObj);

            TSharedPtr<FJsonValueObject> TempValueDataLT = MakeShareable(new FJsonValueObject(TempObjectHiddenListTask));
            arrHiddenListTaskObj.Add(TempValueDataLT);
        }

        TempObjectDataQuest->SetArrayField("Array Visible List Task", arrVisibleListTaskObj);
        TempObjectDataQuest->SetArrayField("Array Hidden List Task", arrHiddenListTaskObj);

        TSharedPtr<FJsonValueObject> TempValueObject = MakeShareable(new FJsonValueObject(TempObjectDataQuest));
        arrRootObj.Add(TempValueObject);
    }
    rootObj->SetArrayField("Array Data Quest", arrRootObj);

    FString Result;
    const TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<TCHAR>::Create(&Result);
    FJsonSerializer::Serialize(rootObj.ToSharedRef(), jsonWriter);

    return Result;
}

void UQuestManagerBase::AddNewArrayDataQuest(const TArray<FDataQuest>& NewData)
{
    if (!CHECK_COND(GetOwner()->HasAuthority(), "Function called on client ???")) return;

    ArrayDataQuest = NewData;
}

void UQuestManagerBase::UpdateInfoDataQuest(const FName& NameQuest)
{
    FDataQuest& DataQuest = GetDataQuestFromName(NameQuest);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Empty data quest")) return;
    if (!CHECK_COND(DataQuest.ActiveVisibleListTask != nullptr, "Active visible list task is nullptr")) return;

    FDataVisibleListTask& DataVisibleListTask = GetDataVisibleListFromListTask(NameQuest, DataQuest.ActiveVisibleListTask);
    if (!CHECK_COND(DataVisibleListTask != EmptyDataVisibleListTask, "Data visible list task is empty")) return;

    DataVisibleListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(DataQuest.ActiveVisibleListTask);
}

void UQuestManagerBase::ClientSendNotifyStartQuest_Implementation(const FName& NameQuest)
{
    OnStartQuest.Broadcast(NameQuest);
}

void UQuestManagerBase::ClientSendNotifyUpdateQuest_Implementation(const FName& NameQuest)
{
    OnUpdateQuest.Broadcast(NameQuest);
}

void UQuestManagerBase::ClientSendNotifyCompleteQuest_Implementation(const FName& NameQuest)
{
    OnCompleteQuest.Broadcast(NameQuest);
}

void UQuestManagerBase::ClientSendNotifySwitchQuest_Implementation(const FName& NameQuest)
{
    OnSwitchQuest.Broadcast(NameQuest);
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

FDataQuest& UQuestManagerBase::FindDataQuestFromID(const uint32 ID)
{
    const auto FindElem = ArrayDataQuest.FindByPredicate([ID](const FDataQuest& DataQuest)
    {
        return DataQuest.ActiveVisibleListTask->GetUniqueID() == ID;
    });

    return FindElem ? *FindElem : EmptyDataQuest;
}

const FDataQuest& UQuestManagerBase::GetFreezeDataQuestFromName(const FName& NameQuest) const
{
    const auto FindElem = ArrayDataQuest.FindByPredicate([NameQuest](const FDataQuest& DataQuest)
    {
        return DataQuest.NameQuestTable.IsEqual(NameQuest);
    });

    return FindElem ? *FindElem : EmptyDataQuest;
}

FDataQuest& UQuestManagerBase::GetDataQuestFromName(const FName& NameQuest)
{
    const auto FindElem = ArrayDataQuest.FindByPredicate([NameQuest](const FDataQuest& DataQuest)
    {
        return DataQuest.NameQuestTable.IsEqual(NameQuest);
    });

    return FindElem ? *FindElem : EmptyDataQuest;
}

FDataQuest& UQuestManagerBase::GetDataQuestFromListTask(const UListTaskBase* ListTask)
{
    const auto FindElem = ArrayDataQuest.FindByPredicate([ListTask](const FDataQuest& DataQuest)
    {
        return DataQuest.ActiveVisibleListTask == ListTask;
    });

    return FindElem ? *FindElem : EmptyDataQuest;
}

FDataQuest& UQuestManagerBase::GetDataQuestFromHiddenLT(const UListTaskBase* ListTask)
{
    for (auto& DataQuest : ArrayDataQuest)
    {
        for (const auto& DataHiddenLT : DataQuest.ArrayHiddenListTasks)
        {
            if (DataHiddenLT.ActiveHiddenListTask == ListTask)
                return DataQuest;
        }
    }
    return EmptyDataQuest;
}

FDataVisibleListTask& UQuestManagerBase::GetDataVisibleListFromListTask(const FName& NameQuest, const UListTaskBase* ListTask)
{
    FDataQuest& DataQuest = GetDataQuestFromName(NameQuest);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return EmptyDataVisibleListTask;
    const auto FindElem = DataQuest.ArrayDataListTask.FindByPredicate([ListTask](const FDataVisibleListTask& VisibleListTask)
    {
        return VisibleListTask.PathToVisibleListTask.GetAssetPathString() == ListTask->GetClass()->GetPathName() && VisibleListTask.bListTaskComplete == false;
    });

    return FindElem ? *FindElem : EmptyDataVisibleListTask;
}

FDataHiddenListTask& UQuestManagerBase::GetDataHiddenListFromListTask(const FName& NameQuest, const UListTaskBase* ListTask)
{
    FDataQuest& DataQuest = GetDataQuestFromName(NameQuest);
    if (!CHECK_COND(DataQuest != EmptyDataQuest, "Data quest is empty")) return EmptyDataHiddenListTask;

    for (auto& DataHiddenLT : DataQuest.ArrayHiddenListTasks)
    {
        if (DataHiddenLT.ActiveHiddenListTask == ListTask)
            return DataHiddenLT;
    }

    return EmptyDataHiddenListTask;
}

#pragma endregion
