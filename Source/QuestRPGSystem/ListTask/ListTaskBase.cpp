// Fill out your copyright notice in the Description page of Project Settings.


#include "ListTask/ListTaskBase.h"
#include "Components/QuestManager.h"
#include "Librarys/QuestLibrary.h"
#include "Net/UnrealNetwork.h"

#pragma region LogListTask

void UListTaskBase::Print_LogListTask(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function) const
{
    if (!OwnerController || !ParentQuestManager) return;
    const FString StrNetMode = UQuestLibrary::GetEnumValueAsString("ENetMode", OwnerController->GetNetMode());
    UQuestLibrary::Print_Log(LogVerb, FString::Printf(TEXT("NetMode: [%s] | OwnerController: [%s] | Parent Quest manager: [%s] | Name: [%s] | %s"),
        *StrNetMode, *OwnerController->GetName(), *ParentQuestManager->GetName(), *GetName(), *Text), Line, Function);
}

#pragma endregion

#pragma region Default

UListTaskBase::UListTaskBase()
{
    
}

bool UListTaskBase::RunListTask(APlayerController* PlayerController, UQuestManager* QuestManager)
{
    if (!CHECK_COND(PlayerController != nullptr, "Player controller is nullptr")) return false;
    if (!CHECK_COND(QuestManager != nullptr, "Parent quest manager is nullptr")) return false;

    OwnerController = PlayerController;
    ParentQuestManager = QuestManager;
    StatusListTask = EStatusListTask::Run;
    return true;
}

int32 UListTaskBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
    return (OwnerController ? OwnerController->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool UListTaskBase::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
    LOG_LIST_TASK(ELogVerb::Display, FString::Printf(TEXT("CallRemoteFunction: %s"), *Function->GetName()));

    UNetDriver* NetDriver = OwnerController->GetNetDriver();
    if (!NetDriver)
    {
        LOG_LIST_TASK(ELogVerb::Error, "Net driver is nullptr");
        return false;
    }

    NetDriver->ProcessRemoteFunction(OwnerController, Function, Parms, OutParms, Stack, this);
    return true;
}

void UListTaskBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION(UListTaskBase, TypeRunListTask, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UListTaskBase, ArrayTask, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UListTaskBase, StatusListTask, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UListTaskBase, OwnerController, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UListTaskBase, ParentQuestManager, COND_OwnerOnly);
}

#pragma endregion