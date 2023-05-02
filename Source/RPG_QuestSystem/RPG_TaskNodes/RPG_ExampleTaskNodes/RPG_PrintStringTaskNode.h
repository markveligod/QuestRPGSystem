/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_TaskNodes/RPG_TaskNodeDataTypes.h"
#include "RPG_PrintStringTaskNode.generated.h"

/**
 * @class The task of rendering the debug lines
 */
UCLASS(DisplayName = "PrintDebugString")
class RPG_QUESTSYSTEM_API URPG_PrintStringTaskNode : public URPG_TaskNodeBase
{
    GENERATED_BODY()

#pragma region DefaultInterfaceTaskNodeBase

public:
    
    URPG_PrintStringTaskNode();

    /** @public Return a one line description of an object for viewing in the thumbnail view of the generic browser */
    virtual FString GetDesc() override { return FString("PrintDebugString"); }

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

    /** @private Action on printing a line on the screen**/
    UFUNCTION(Client, Unreliable)
    void ActionPrintString();

    /** @private Call back complete task on Server side **/
    UFUNCTION(Server, Reliable)
    void ServerCompleteTask();

#pragma endregion

#pragma region TaskSettings

private:

    /** @private The line to be printed **/
    UPROPERTY(EditAnywhere)
    FString DisplayString{""};

    /** @private Enable logging in the console **/
    UPROPERTY(EditAnywhere)
    bool bEnableToLog{false};

    /** @private Enable logging to the screen **/
    UPROPERTY(EditAnywhere)
    bool bEnableToDisplay{true};

    /** @private The minimum set of data for rendering on the screen **/
    UPROPERTY(EditAnywhere, meta = (EditCondition = "bEnableToDisplay", EditConditionHides))
    FRPG_DrawDebugInfoMinimal DrawDebugInfoMinimal;

#pragma endregion
};
