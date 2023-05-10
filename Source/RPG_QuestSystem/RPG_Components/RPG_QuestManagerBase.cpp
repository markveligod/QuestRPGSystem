/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestManagerBase.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "RPG_Config/RPG_QuestSystemSettings.h"
#include "RPG_Librarys/RPG_QuestSystemLibrary.h"
#include "RPG_QuestObjects/RPG_QuestObjectBase.h"

#pragma region ConsoleAction

static TAutoConsoleVariable<bool> EnableD_ShowStateQuest(TEXT("RPGQuestSystem.ShowStateQuest"), false, TEXT("RPGQuestSystem.ShowStateQuest [true/false]"), ECVF_Cheat);

static FAutoConsoleCommandWithWorldAndArgs EnableD_AddQuest(TEXT("RPGQuestSystem.AddQuest"), TEXT("RPGQuestSystem.AddQuest [QuestTableName]"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString>& Args, UWorld* World)
        {
            if (!World) return;
            if (!Args.IsValidIndex(0)) return;
            FName QuestName = FName(Args[0]);
            APlayerController* PC = World->GetFirstPlayerController();
            if (!PC) return;
            URPG_QuestManagerBase* QM = PC->FindComponentByClass<URPG_QuestManagerBase>();
            if (!QM) return;
            QM->ServerAddQuest(QuestName);
        }));

static FAutoConsoleCommandWithWorldAndArgs EnableD_RemoveQuest(TEXT("RPGQuestSystem.RemoveQuest"), TEXT("RPGQuestSystem.RemoveQuest [QuestTableName]"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString>& Args, UWorld* World)
        {
            if (!World) return;
            if (!Args.IsValidIndex(0)) return;
            FName QuestName = FName(Args[0]);
            APlayerController* PC = World->GetFirstPlayerController();
            if (!PC) return;
            URPG_QuestManagerBase* QM = PC->FindComponentByClass<URPG_QuestManagerBase>();
            if (!QM) return;
            QM->ServerRemoveQuest(QuestName);
        }));

#pragma endregion

#pragma region Log

void URPG_QuestManagerBase::Print_LogQuestManger(const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const
{
    if (QUEST_MANAGER_CLOG(!OwnerPC, Error, TEXT("Owner is nullptr"))) return;

    const FString NetMode = URPG_QuestSystemLibrary::GetNetModeToString(OwnerPC);
    const FString Result = FString::Printf(TEXT("NetMode: [%s] | Owner Name: [%s] | Text: [%s]"), *NetMode, *OwnerPC->GetName(), *Text);
    URPG_QuestSystemLibrary::Print_Log(LogVerb, Result, Line, Function);
}

bool URPG_QuestManagerBase::Print_CLogQuestManager(const bool bCond, const TEnumAsByte<ERPG_QSLogVerb> LogVerb, const FString& Text, const int Line, const char* Function) const
{
    if (!OwnerPC) return false;

    const FString NetMode = URPG_QuestSystemLibrary::GetNetModeToString(OwnerPC);
    const FString Result = FString::Printf(TEXT("NetMode: [%s] | Owner Name: [%s] | Text: [%s]"), *NetMode, *OwnerPC->GetName(), *Text);
    return URPG_QuestSystemLibrary::Print_CLog(bCond, LogVerb, Result, Line, Function);
}

#pragma endregion

#pragma region DefaultInterface

// Sets default values for this component's properties
URPG_QuestManagerBase::URPG_QuestManagerBase()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bAllowTickOnDedicatedServer = false;

#if !UE_BUILD_SHIPPING
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
#endif

    SetIsReplicatedByDefault(true);
    bReplicateUsingRegisteredSubObjectList = false;
}

// Called every frame
void URPG_QuestManagerBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (EnableD_ShowStateQuest.GetValueOnGameThread())
    {
        for (auto& Data : ArrayDataQuest)
        {
            if (!Data.IsValidQuest()) continue;
            FString NetMode = URPG_QuestSystemLibrary::GetNetModeToString(OwnerPC);
            FString IsActiveQuest = Data.ActiveQuest ? "TRUE" : "FALSE";
            FString Result = FString::Printf(TEXT("NetMode: [%s] | Quest Table Name: [%s] | IsActiveQuest: [%s] | Quest State: [%s]"),
                *NetMode, *Data.QuestRowNameTable.ToString(),
                *IsActiveQuest, *UEnum::GetValueAsString(Data.StateEntity));

            if (Data.ActiveQuest)
            {
                const TArray<FRPG_TaskNodeData>& AllTaskNodes = Data.ActiveQuest->GetArrayTaskNodes();
                for (auto& TaskNode : AllTaskNodes)
                {
                    if (TaskNode.TypeNode != ERPG_TypeNode::StandardNode) continue;
                    if (!TaskNode.TaskNodeBase) continue;
                    if (TaskNode.TaskNodeBase->GetStateTaskNode() != ERPG_StateEntity::Run) continue;

                    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, TaskNode.ToString());
                }
            }
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, Result);
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, "--- | --- | --- | ---");
        }
    }
}

void URPG_QuestManagerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(URPG_QuestManagerBase, ArrayDataQuest, COND_OwnerOnly, REPNOTIFY_Always);
}

bool URPG_QuestManagerBase::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    // Replicated quests and task from quests
    if (!QueueRepQuest.IsEmpty())
    {
        FName QuestName;
        QueueRepQuest.Dequeue(QuestName);
        FRPG_DataQuest* DataQuest = FindDataQuestByName(QuestName);
        if (DataQuest && DataQuest->ActiveQuest)
        {
            WroteSomething |= Channel->ReplicateSubobject(DataQuest->ActiveQuest, *Bunch, *RepFlags);
            TArray<URPG_TaskNodeBase*> ArrayTasks = DataQuest->ActiveQuest->GetArrayInstanceTaskNodes();
            for (URPG_TaskNodeBase* Task : ArrayTasks)
            {
                WroteSomething |= Channel->ReplicateSubobject(Task, *Bunch, *RepFlags);
            }
        }
    }

    return WroteSomething;
}

// Called when the game starts
void URPG_QuestManagerBase::BeginPlay()
{
    Super::BeginPlay();

    OwnerPC = Cast<APlayerController>(GetOwner());
    if (QUEST_MANAGER_CLOG(!OwnerPC, Error, TEXT("Owner is nullptr"))) return;
    OwnerPC->SetReplicates(true);
}

#pragma endregion

#pragma region ActionQuestManager

void URPG_QuestManagerBase::ServerAddQuest_Implementation(const FName& NewQuest)
{
    AddQuest(NewQuest);
}

bool URPG_QuestManagerBase::ServerAddQuest_Validate(const FName& NewQuest)
{
    return IsValidationRequestAddQuest(NewQuest);
}

void URPG_QuestManagerBase::AddQuest(FName NewQuest)
{
    if (QUEST_MANAGER_CLOG(!OwnerPC, Error, TEXT("Owner is nullptr"))) return;
    if (QUEST_MANAGER_CLOG(!OwnerPC->HasAuthority(), Error, TEXT("Owner is not Authority"))) return;
    if (QUEST_MANAGER_CLOG(!DataQuestTable, Error, TEXT("Data quest table is nullptr"))) return;
    if (QUEST_MANAGER_CLOG(NewQuest == NAME_None, Error, TEXT("Name quest is none"))) return;
    if (QUEST_MANAGER_CLOG(!DataQuestTable->GetRowNames().Contains(NewQuest), Error, TEXT("Name quest is not contains in DataQuestTable"))) return;
    if (QUEST_MANAGER_CLOG(FindDataQuestByName(NewQuest) != nullptr, Error, TEXT("Quest is exist"))) return;

    FRPG_DataQuestTable* FindDataQuestTable = DataQuestTable->FindRow<FRPG_DataQuestTable>(NewQuest, TEXT(""));
    if (QUEST_MANAGER_CLOG(FindDataQuestTable == nullptr, Error, TEXT("FindDataQuestTable is nullptr"))) return;

    FRPG_DataQuest NewDataQuest;
    NewDataQuest.QuestRowNameTable = NewQuest;
    URPG_QuestObjectBase* RefQuestObject = FindDataQuestTable->QuestObjectPath.LoadSynchronous();
    if (!RefQuestObject) return;
    NewDataQuest.ActiveQuest = NewObject<URPG_QuestObjectBase>(this, RefQuestObject->GetClass(), NAME_None, RF_NoFlags, RefQuestObject);
    if (QUEST_MANAGER_CLOG(!NewDataQuest.IsValidQuest(), Error, TEXT("Quest is not valid"))) return;

    NewDataQuest.ActiveQuest->OnUpdateStateQuest.BindUObject(this, &ThisClass::RegisterUpdateQuest_Event);
    if (NewDataQuest.ActiveQuest->InitQuest(OwnerPC, this))
    {
        ArrayDataQuest.AddUnique(NewDataQuest);
        NewDataQuest.ActiveQuest->OnUpdateStateQuest.BindUObject(this, &ThisClass::RegisterUpdateQuest_Event);
        NewDataQuest.ActiveQuest->RunQuest();
    }
    NotifyUpdateQuest(NewQuest);
}

void URPG_QuestManagerBase::ServerRemoveQuest_Implementation(const FName& QuestName)
{
    RemoveQuest(QuestName);
}

bool URPG_QuestManagerBase::ServerRemoveQuest_Validate(const FName& QuestName)
{
    return IsValidationRequestRemoveQuest(QuestName);
}

void URPG_QuestManagerBase::RemoveQuest(FName QuestName)
{
    if (QUEST_MANAGER_CLOG(!OwnerPC, Error, TEXT("Owner is nullptr"))) return;
    if (QUEST_MANAGER_CLOG(!OwnerPC->HasAuthority(), Error, TEXT("Owner is not Authority"))) return;
    if (QUEST_MANAGER_CLOG(QuestName == NAME_None, Error, TEXT("Name quest is none"))) return;

    FRPG_DataQuest* DataQuest = FindDataQuestByName(QuestName);
    if (QUEST_MANAGER_CLOG(DataQuest == nullptr, Warning, TEXT("DataQuest is nullptr"))) return;
    
    DestroyActiveQuestByName(QuestName);
    ArrayDataQuest.RemoveAll([QuestName](const FRPG_DataQuest& Data)
    {
        return Data.QuestRowNameTable.IsEqual(QuestName);
    });
}

FRPG_DataQuest* URPG_QuestManagerBase::FindDataQuestByName(const FName& CheckQuest)
{
    return ArrayDataQuest.FindByPredicate([CheckQuest](const FRPG_DataQuest& Data) { return Data.QuestRowNameTable.IsEqual(CheckQuest); });
}

int32 URPG_QuestManagerBase::FindIndexDataQuestByName(const FName& CheckQuest) const
{
    return ArrayDataQuest.IndexOfByPredicate([CheckQuest](const FRPG_DataQuest& Data) { return Data.QuestRowNameTable.IsEqual(CheckQuest); });
}

FRPG_DataQuest& URPG_QuestManagerBase::FindDataQuestByQuestObject(URPG_QuestObjectBase* QuestObject)
{
    const int32 ElemIndex = ArrayDataQuest.IndexOfByPredicate([QuestObject](const FRPG_DataQuest& Data) { return Data.ActiveQuest == QuestObject; });
    if (ElemIndex == INDEX_NONE) return EMPTY_DATA_QUEST;

    return ArrayDataQuest[ElemIndex];
}

const FRPG_DataQuest& URPG_QuestManagerBase::FindFreezeDataQuestByQuestObject(URPG_QuestObjectBase* QuestObject)
{
    const int32 ElemIndex = ArrayDataQuest.IndexOfByPredicate([QuestObject](const FRPG_DataQuest& Data) { return Data.ActiveQuest == QuestObject; });
    if (ElemIndex == INDEX_NONE) return EMPTY_DATA_QUEST;

    return ArrayDataQuest[ElemIndex];
}

ERPG_StateEntity URPG_QuestManagerBase::GetStateQuestByName(FName QuestName)
{
    const FRPG_DataQuest* DataQuest = FindDataQuestByName(QuestName);
    if (DataQuest && DataQuest->IsValidQuest())
    {
        return DataQuest->StateEntity;
    }
    return ERPG_StateEntity::None;
}

bool URPG_QuestManagerBase::IsValidationRequestAddQuest(const FName& CheckQuest)
{
    return CheckQuest != NAME_None && !FindDataQuestByName(CheckQuest);
}

bool URPG_QuestManagerBase::IsValidationRequestRemoveQuest(const FName& CheckQuest)
{
    return FindDataQuestByName(CheckQuest) != nullptr;
}

void URPG_QuestManagerBase::OnRep_ArrayDataQuest()
{
    for (auto& DataQuest : ArrayDataQuest)
    {
        NotifyUpdateQuest(DataQuest.QuestRowNameTable);
    }
}

void URPG_QuestManagerBase::RegisterUpdateQuest_Event(URPG_QuestObjectBase* QuestObject)
{
    if (QUEST_MANAGER_CLOG(!QuestObject, Error, TEXT("Quest object is not valid"))) return;

    FRPG_DataQuest& DataQuest = FindDataQuestByQuestObject(QuestObject);
    if (QUEST_MANAGER_CLOG(!DataQuest.IsValidQuest(), Error, TEXT("Quest is not valid"))) return;

    DataQuest.StateEntity = QuestObject->GetStateQuest();
    const FTimerDelegate NotifyUpdateQuestTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::NotifyUpdateQuest, DataQuest.QuestRowNameTable);
    GetWorld()->GetTimerManager().SetTimerForNextTick(NotifyUpdateQuestTimerDelegate);
    QueueRepQuest.Enqueue(DataQuest.QuestRowNameTable);

    if (QuestObject->GetStateQuest() == ERPG_StateEntity::Complete)
    {
        QuestObject->OnUpdateStateQuest.Unbind();

        if (const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>())
        {
            FTimerHandle DestroyActiveQuestTimerHandle;
            const FTimerDelegate DestroyActiveQuestTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::DestroyActiveQuestByName, DataQuest.QuestRowNameTable);
            GetWorld()->GetTimerManager().SetTimer(DestroyActiveQuestTimerHandle, DestroyActiveQuestTimerDelegate, QuestSystemSettings->DelayToQuestDestroy, false);
        }
    }
}

void URPG_QuestManagerBase::NotifyUpdateQuest(const FName NameQuest)
{
    FRPG_DataQuest* DataQuest = FindDataQuestByName(NameQuest);
    if (QUEST_MANAGER_CLOG(!DataQuest, Error, TEXT("DataQuest is nullptr"))) return;
    if (QUEST_MANAGER_CLOG(!DataQuest->IsValidQuest(), Error, TEXT("DataQuest is not valid"))) return;
    if (QUEST_MANAGER_CLOG(DataQuest->ActiveQuest == nullptr, Warning, TEXT("ActiveQuest is nullptr"))) return;

    APlayerController* LocalOwner = IsNetMode(NM_Client) ? GetWorld()->GetFirstPlayerController() : OwnerPC;
    OnUpdateDataQuest.Broadcast(LocalOwner, DataQuest->QuestRowNameTable, DataQuest->StateEntity);
}

void URPG_QuestManagerBase::DestroyActiveQuestByName(FName NameQuest)
{
    FRPG_DataQuest* DataQuest = FindDataQuestByName(NameQuest);
    if (QUEST_MANAGER_CLOG(!DataQuest, Error, TEXT("DataQuest is nullptr"))) return;
    if (QUEST_MANAGER_CLOG(!DataQuest->ActiveQuest, Warning, TEXT("ActiveQuest is nullptr"))) return;
    DataQuest->ActiveQuest->OnUpdateStateQuest.Unbind();
    DataQuest->ActiveQuest->ResetQuest();
    DataQuest->ActiveQuest->MarkAsGarbage();
    DataQuest->ActiveQuest = nullptr;
}

#pragma endregion
