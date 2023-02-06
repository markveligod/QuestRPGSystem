// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestManagerBase.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Json.h"

#if UE_EDITOR || UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG

static TAutoConsoleVariable<int32> EnableD_QuestSystemSelectQuest(TEXT("QuestSystem.ShowSelectQuest"), INDEX_NONE, TEXT("The enabled parameter shows information about the selected quest"), ECVF_Cheat);
static TAutoConsoleVariable<FString> EnableD_QuestSystemShow3DQuest(TEXT("QuestSystem.Show3DQuest"), "", TEXT("Show 3D data for rendering the work of the quest. Example: QuestSystem.Show3DQuest <NameQuestTable>"), ECVF_Cheat);
static TAutoConsoleVariable<float> EnableD_QuestSystemSizeDebug(TEXT("QuestSystem.SetSizeDebug"), 1.0f, TEXT("Setup size debug "), ECVF_Cheat);
static TAutoConsoleVariable<bool> EnableD_QuestSystemAllQuest(TEXT("QuestSystem.ShowAllQuest"), false, TEXT("The enabled parameter shows information about the all quest"), ECVF_Cheat);


#endif

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

#if UE_EDITOR || UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG

    if (EnableD_QuestSystemAllQuest.GetValueOnGameThread())
    {
        if (!GetOwner()) return;

        for (int32 i = 0; i < ArrayDataQuest.Num(); ++i)
        {
            if (!ArrayDataQuest.IsValidIndex(i)) continue;
            const FDataQuest& DataQuest = ArrayDataQuest[i];
            FString Result = FString::Printf(TEXT("Net mode: [%s] | Index: [%i] | QuestTableName: [%s] | StatusQuest: [%s]"),
                *UQuestLibrary::GetNetModeToString(GetOwner()), i, *DataQuest.NameQuestTable.ToString(), *UEnum::GetValueAsString(DataQuest.StatusQuest));
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, Result, true, FVector2D(EnableD_QuestSystemSizeDebug.GetValueOnGameThread()));
        }
    }

    if (ArrayDataQuest.IsValidIndex(EnableD_QuestSystemSelectQuest.GetValueOnGameThread()))
    {
        if (!GetOwner()) return;

        const FString Result = FString::Printf(TEXT("Net mode: [%s] | DataQuest: [%s]"),
            *UQuestLibrary::GetNetModeToString(GetOwner()), *ArrayDataQuest[EnableD_QuestSystemSelectQuest.GetValueOnGameThread()].ToString());
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, Result, true, FVector2D(EnableD_QuestSystemSizeDebug.GetValueOnGameThread()));
    }

    if (!EnableD_QuestSystemShow3DQuest.GetValueOnGameThread().IsEmpty())
    {
        
    }

#endif
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



FDataQuest& UQuestManagerBase::FindDataQuestFromID(const uint32 ID)
{
    return  EmptyDataQuest;
}

void UQuestManagerBase::DestroyActiveListTaskFromQuestName(const FName& NameQuest)
{
    FDataQuest& DataQuest = GetDataQuestFromName(NameQuest);
    if (!CHECK_COND(!DataQuest.IsEmpty(), "Data quest is empty")) return;
    
}

void UQuestManagerBase::DestroyActiveListTaskForAllQuest()
{
    for (auto& DataQuest : ArrayDataQuest)
    {
        
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

#pragma endregion
