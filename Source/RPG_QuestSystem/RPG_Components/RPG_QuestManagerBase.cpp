/** Copyright Mark Veligod. Published in 2023. **/

#include "RPG_QuestManagerBase.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "RPG_Config/RPG_QuestSystemSettings.h"
#include "RPG_Librarys/RPG_QuestSystemLibrary.h"
#include "RPG_QuestObjects/RPG_QuestObjectBase.h"

#pragma region ConsoleAction

static TAutoConsoleVariable<bool> EnableD_ShowStateQuest(TEXT("RPGQuestSystem.ShowStateQuest"),false,TEXT("RPGQuestSystem.ShowStateQuest [true/false]"),ECVF_Cheat);

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
            FString Result = FString::Printf(TEXT("Quest Table Name: [%s] | Quest State: [%s]"),
                *Data.QuestRowNameTable.ToString(), *UEnum::GetValueAsString(Data.ActiveQuest->GetStateQuest()));
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, Result);
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
    
    if (!QueueRepQuest.IsEmpty())
    {
        FName QuestName;
        QueueRepQuest.Dequeue(QuestName);
        FRPG_DataQuest* DataQuest = FindDataQuestByName(QuestName);
        if (DataQuest && DataQuest->IsValidQuest())
        {
            WroteSomething |= Channel->ReplicateSubobject(DataQuest->ActiveQuest, *Bunch, *RepFlags);
            TArray<URPG_TaskNodeBase*> AllTasks = DataQuest->ActiveQuest->GetArrayInstanceTaskNodes();
            for (URPG_TaskNodeBase* Task : AllTasks)
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
    if (QUEST_MANAGER_CLOG(FindDataQuestByName(NewQuest) != nullptr, Error, TEXT("Quest is exist"))) return;

    const URPG_QuestSystemSettings* QuestSystemSettings = GetDefault<URPG_QuestSystemSettings>();
    if (QUEST_MANAGER_CLOG(!QuestSystemSettings, Error, TEXT("QuestSystemSettings is nullptr"))) return;

    FRPG_DataQuest NewDataQuest;
    NewDataQuest.QuestRowNameTable = NewQuest;
    NewDataQuest.ActiveQuest = NewObject<URPG_QuestObjectBase>(this, QuestSystemSettings->SupportQuestClass);
    if (QUEST_MANAGER_CLOG(!NewDataQuest.IsValidQuest(), Error, TEXT("Quest is not valid"))) return;

    NewDataQuest.ActiveQuest->OnUpdateStateQuest.BindUObject(this, &ThisClass::RegisterUpdateQuest_Event);
    ArrayDataQuest.AddUnique(NewDataQuest);
}

FRPG_DataQuest* URPG_QuestManagerBase::FindDataQuestByName(const FName& CheckQuest)
{
    return ArrayDataQuest.FindByPredicate([CheckQuest](const FRPG_DataQuest& Data)
    {
        return Data.QuestRowNameTable.IsEqual(CheckQuest);
    });
}

int32 URPG_QuestManagerBase::FindIndexDataQuestByName(const FName& CheckQuest) const
{
    return ArrayDataQuest.IndexOfByPredicate([CheckQuest](const FRPG_DataQuest& Data)
    {
        return Data.QuestRowNameTable.IsEqual(CheckQuest);
    });
}

const FRPG_DataQuest& URPG_QuestManagerBase::FindDataQuestByQuestObject(URPG_QuestObjectBase* QuestObject)
{
    const int32 ElemIndex = ArrayDataQuest.IndexOfByPredicate([QuestObject](const FRPG_DataQuest& Data)
    {
        return Data.ActiveQuest == QuestObject;
    });
    if (ElemIndex == INDEX_NONE) return EMPTY_DATA_QUEST;

    return ArrayDataQuest[ElemIndex];
}

bool URPG_QuestManagerBase::IsValidationRequestAddQuest(const FName& CheckQuest)
{
    return CheckQuest != NAME_None && !FindDataQuestByName(CheckQuest);
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

    const FRPG_DataQuest& DataQuest = FindDataQuestByQuestObject(QuestObject);
    const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::NotifyUpdateQuest, DataQuest.QuestRowNameTable);
    GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
    QueueRepQuest.Enqueue(DataQuest.QuestRowNameTable);
}

void URPG_QuestManagerBase::NotifyUpdateQuest(const FName NameQuest)
{
    const FRPG_DataQuest* DataQuest = FindDataQuestByName(NameQuest);
    if (QUEST_MANAGER_CLOG(!DataQuest, Error, TEXT("DataQuest is nullptr"))) return;
    if (QUEST_MANAGER_CLOG(!DataQuest->IsValidQuest(), Error, TEXT("DataQuest is not valid"))) return;

    APlayerController* LocalOwner = IsNetMode(NM_Client) ? GetWorld()->GetFirstPlayerController() : OwnerPC;
    OnUpdateDataQuest.Broadcast(LocalOwner, DataQuest->QuestRowNameTable, DataQuest->ActiveQuest->GetStateQuest());
}

#pragma endregion
