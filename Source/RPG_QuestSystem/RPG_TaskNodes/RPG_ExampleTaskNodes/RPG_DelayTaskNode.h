/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_DelayTaskNode.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Delay")
class RPG_QUESTSYSTEM_API URPG_DelayTaskNode : public URPG_TaskNodeBase
{
	GENERATED_BODY()

#pragma region DefaultInterfaceTaskNodeBase

public:
    
    URPG_DelayTaskNode();

    /** @public Return a one line description of an object for viewing in the thumbnail view of the generic browser */
    virtual FString GetDesc() override { return FString("Delay"); }

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

#pragma region DataTask

private:

    /** @private Delay time **/
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1", ForceUnits = "s"))
    float DelayTime{1.0f};

#pragma endregion

};
