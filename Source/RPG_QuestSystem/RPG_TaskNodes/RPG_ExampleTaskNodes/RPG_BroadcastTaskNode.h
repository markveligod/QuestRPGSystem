// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_BroadcastTaskNode.generated.h"

/**
 * @class Task for tag forwarding
 */
UCLASS(DisplayName = "BroadcastTask")
class RPG_QUESTSYSTEM_API URPG_BroadcastTaskNode : public URPG_TaskNodeBase
{
	GENERATED_BODY()

#pragma region DefaultInterfaceTaskNodeBase

public:
    URPG_BroadcastTaskNode();

    /** @public Return a one line description of an object for viewing in the thumbnail view of the generic browser */
    virtual FString GetDesc() override { return FString("BroadcastTask"); }

protected:
    /** @protected Initializing a task **/
    virtual bool InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest) override;

    /** @protected Run a task **/
    virtual void RunTask() override;

    /** @protected Complete a task **/
    virtual void CompleteTask() override;

    /** @protected Reset a task **/
    virtual void ResetTask() override;

#pragma endregion

#pragma region ActionTask

private:

    void ActionBroadcast();

#pragma endregion

#pragma region DataTask

private:

    UPROPERTY(EditAnywhere)
    FGameplayTag Tag;

#pragma endregion
    
};
