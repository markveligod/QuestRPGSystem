/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_InstanceTaskNode.generated.h"

/**
 *
 */
UCLASS(DisplayName = "Instance Task")
class RPG_QUESTSYSTEM_API URPG_InstanceTaskNode : public URPG_TaskNodeBase
{
    GENERATED_BODY()

#pragma region DefaultInterfaceTaskNodeBase

public:
    URPG_InstanceTaskNode();

    /** @public Return a one line description of an object for viewing in the thumbnail view of the generic browser */
    virtual FString GetDesc() override { return FString("InstanceTask"); }

protected:
    /** @protected Initializing a task **/
    virtual bool InitTask(APlayerController* PlayerController, URPG_QuestObjectBase* ParentQuest) override;

    /** @protected Run a task **/
    virtual void RunTask() override;

    /** @protected Complete a task **/
    virtual void CompleteTask() override;

    /** @protected Reset a task **/
    virtual void ResetTask() override;

    /** @protected Get all instance tasks**/
    virtual TArray<URPG_TaskNodeBase*> GetAllInstanced() override;

#pragma endregion

#pragma region DataTask

private:
    UPROPERTY(EditAnywhere, Instanced)
    TArray<URPG_TaskNodeBase*> ExecuteTasks;

#pragma endregion

#pragma region ActionTask

private:
    void RegisterUpdateTask(URPG_TaskNodeBase* Task);

#pragma endregion
};
