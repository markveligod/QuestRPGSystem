// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestManagerBase.h"
#include "Engine/ActorChannel.h"
#include "ListTask/ListTaskBase.h"
#include "Net/UnrealNetwork.h"
#include "Json.h"

#pragma region LogQuestManager

void UQuestManagerBase::Print_LogQuestManager(const TEnumAsByte<EQuestLogVerb> LogVerb, const FString Text, const int Line, const char* Function) const
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

    SetIsReplicatedByDefault(true);
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
            LOG_QM(Display, FString::Printf(TEXT("List Task: [%s] replicated by ID: %lu"), *ListTask->GetName(), TempUniqID));
            WroteSomething |= Channel->ReplicateSubobject(ListTask, *Bunch, *RepFlags);
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
    if (!CHECK_COND(!DataQuest.IsEmpty(), "Data quest is empty")) return;
    if (!CHECK_COND(DataQuest.ActiveListTask != nullptr, "Active list task is nullptr")) return;

    QueuePushReplicateObject.Enqueue(DataQuest.ActiveListTask->GetUniqueID());
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

        // Visible list task
        TArray<TSharedPtr<FJsonValue>> arrVisibleListTaskObj;
        for (const auto& DataVisibleListTask : DataQuest.ArrayDataListTask)
        {
            const TSharedPtr<FJsonObject> TempObjectVisibleListTask = MakeShareable(new FJsonObject());
            TempObjectVisibleListTask->SetStringField("PathToListTask", DataVisibleListTask.PathToListTask.ToString());
            TempObjectVisibleListTask->SetBoolField("bListTaskComplete", DataVisibleListTask.bListTaskComplete);

            TArray<TSharedPtr<FJsonValue>> arrTaskObj;
            for (const auto& TaskInfo : DataVisibleListTask.ArrayDataTask)
            {
                const TSharedPtr<FJsonObject> TempObjectTaskInfo = MakeShareable(new FJsonObject());
                TempObjectTaskInfo->SetStringField("TaskID", TaskInfo.TaskID);
                TempObjectTaskInfo->SetStringField("TaskDescription", TaskInfo.TaskDescription.ToString());
                TempObjectTaskInfo->SetBoolField("bStatusTask", TaskInfo.bStatusTask);

                TSharedPtr<FJsonValueObject> TempValueTaskInfo = MakeShareable(new FJsonValueObject(TempObjectTaskInfo));
                arrTaskObj.Add(TempValueTaskInfo);
            }
            TempObjectVisibleListTask->SetArrayField("Array Task info", arrTaskObj);

            TSharedPtr<FJsonValueObject> TempValueDataLT = MakeShareable(new FJsonValueObject(TempObjectVisibleListTask));
            arrVisibleListTaskObj.Add(TempValueDataLT);
        }
        
        TempObjectDataQuest->SetArrayField("Array List Task", arrVisibleListTaskObj);

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
    const FDataQuest& DataQuest = GetDataQuestFromName(NameQuest);
    if (!CHECK_COND(!DataQuest.IsEmpty(), "Empty data quest")) return;
    if (!CHECK_COND(DataQuest.ActiveListTask != nullptr, "Active visible list task is nullptr")) return;
    
    FDataListTask& DataListTask = GetDataListTaskFromPathListTask(DataQuest.ActiveListTask);
    if (!CHECK_COND(!DataListTask.IsEmpty(), "Data visible list task is empty")) return;
    
    DataListTask.ArrayDataTask = UQuestLibrary::FillDataInfoTasksFromListTask(DataQuest.ActiveListTask);
}

void UQuestManagerBase::ClientSendNotifyStartQuest_Implementation(const FName& NameQuest)
{
    SendNotifyStartQuest(NameQuest);
}

void UQuestManagerBase::ClientSendNotifyUpdateQuest_Implementation(const FName& NameQuest)
{
    SendNotifyUpdateQuest(NameQuest);
}

void UQuestManagerBase::ClientSendNotifyCompleteQuest_Implementation(const FName& NameQuest)
{
    SendNotifyCompleteQuest(NameQuest);
}

void UQuestManagerBase::SendNotifyStartQuest(const FName& NameQuest)
{
    OnStartQuest.Broadcast(NameQuest);
    SendNotifyStartQuest_Event(NameQuest);
}

void UQuestManagerBase::SendNotifyUpdateQuest(const FName& NameQuest)
{
    OnUpdateQuest.Broadcast(NameQuest);
    SendNotifyUpdateQuest_Event(NameQuest);
}

void UQuestManagerBase::SendNotifyCompleteQuest(const FName& NameQuest)
{
    OnCompleteQuest.Broadcast(NameQuest);
    SendNotifyCompleteQuest_Event(NameQuest);
}

#pragma endregion

#pragma region FindData

UListTaskBase* UQuestManagerBase::FindListTaskFromID(const uint32 ID) const
{
    const auto FindElem = ArrayDataQuest.FindByPredicate([ID](const FDataQuest& DataQuest)
    {
        return DataQuest.ActiveListTask && DataQuest.ActiveListTask->GetUniqueID() == ID;
    });

    return FindElem ? FindElem->ActiveListTask : nullptr;
}

FDataQuest& UQuestManagerBase::FindDataQuestFromID(const uint32 ID)
{
    const auto FindElem = ArrayDataQuest.FindByPredicate([ID](const FDataQuest& DataQuest)
    {
        return DataQuest.ActiveListTask && DataQuest.ActiveListTask->GetUniqueID() == ID;
    });

    return FindElem ? *FindElem : EmptyDataQuest;
}

void UQuestManagerBase::DestroyActiveListTaskFromQuestName(const FName& NameQuest)
{
    FDataQuest& DataQuest = GetDataQuestFromName(NameQuest);
    if (!CHECK_COND(!DataQuest.IsEmpty(), "Data quest is empty")) return;

    if (DataQuest.ActiveListTask)
    {
        DataQuest.ActiveListTask->OnUpdateListTask.RemoveAll(this);
        DataQuest.ActiveListTask->DestroyListTask();
        UQuestLibrary::UnLoadListTaskFromPath(DataQuest.ActiveListTask->GetClass()->GetPathName());
        DataQuest.ActiveListTask = nullptr;
    }
}

void UQuestManagerBase::DestroyActiveListTaskForAllQuest()
{
    for (auto& DataQuest : ArrayDataQuest)
    {
        if (DataQuest.ActiveListTask)
        {
            DataQuest.ActiveListTask->OnUpdateListTask.RemoveAll(this);
            DataQuest.ActiveListTask->DestroyListTask();
            DataQuest.ActiveListTask = nullptr;
        }
    }
}

int32 UQuestManagerBase::GetIndexQuestFromName(const FName& InQuestName)
{
    return ArrayDataQuest.IndexOfByPredicate([InQuestName](const FDataQuest& Data)
    {
        return Data.NameQuestTable.IsEqual(InQuestName);
    });
}

const FDataQuest& UQuestManagerBase::GetFreezeDataQuestFromIndex(const int32& InIndex)
{
    if (ArrayDataQuest.IsValidIndex(InIndex))
    {
        return ArrayDataQuest[InIndex];
    }
    return EmptyDataQuest;
}

const FDataQuest& UQuestManagerBase::GetFreezeDataQuestFromName(const FName& InQuestName)
{
    return GetFreezeDataQuestFromIndex(GetIndexQuestFromName(InQuestName));
}

FDataQuestTable UQuestManagerBase::GetDataQuestTableFromName(const FName& NameQuest) const
{
    if (!CHECK_COND(DataQuestTable != nullptr, "Data quest table is nullptr")) return FDataQuestTable();
    FDataQuestTable* QuestFromTable = DataQuestTable->FindRow<FDataQuestTable>(NameQuest, "");
    return QuestFromTable ? *QuestFromTable : FDataQuestTable();
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
    if (!ListTask) return EmptyDataQuest;
    const auto FindElem = ArrayDataQuest.FindByPredicate([ListTask](const FDataQuest& DataQuest)
    {
        return DataQuest.ActiveListTask == ListTask;
    });

    return FindElem ? *FindElem : EmptyDataQuest;
}

FDataQuest& UQuestManagerBase::GetDataQuestFromPathListTask(const FSoftObjectPath& InListTaskPath)
{
    if (InListTaskPath.IsNull()) return EmptyDataQuest;
    const auto FindElem = ArrayDataQuest.FindByPredicate([InListTaskPath](const FDataQuest& DataQuest)
    {
        return DataQuest.ActiveListTask && DataQuest.ActiveListTask->GetClass()->GetName() == InListTaskPath.GetAssetName();
    });

    return FindElem ? *FindElem : EmptyDataQuest;
}

FDataListTask& UQuestManagerBase::GetDataListTaskFromPathListTask(const FSoftObjectPath& InListTaskPath)
{
    if (InListTaskPath.IsNull()) return EmptyDataListTask;
    FDataQuest& DataQuest = GetDataQuestFromPathListTask(InListTaskPath);

    const auto FindElem = DataQuest.ArrayDataListTask.FindByPredicate([InListTaskPath](const FDataListTask& Data)
    {
        return Data.PathToListTask == InListTaskPath;
    });

    return FindElem ? *FindElem : EmptyDataListTask;
}

#pragma endregion
