/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_TaskNodes/RPG_TaskNodeBase.h"
#include "RPG_MoveToTaskNode.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Move TO")
class RPG_QUESTSYSTEM_API URPG_MoveToTaskNode : public URPG_TaskNodeBase
{
	GENERATED_BODY()

#pragma region DefaultInterfaceTaskNodeBase

public:
    
    URPG_MoveToTaskNode();

    /** @public Return a one line description of an object for viewing in the thumbnail view of the generic browser */
    virtual FString GetDesc() override { return FString("MoveTo"); }

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

    void RegisterCheckDistToPos();

#pragma endregion

#pragma region TaskSettings

private:

    /** @private Target point of achievement and completion of the current task **/
    UPROPERTY(EditAnywhere)
    FVector TargetPosition{FVector::ZeroVector};

    /** @private Task completion radius **/
    UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", ForceUnits = "cm"))
    float RadiusComplete{100.0f};

    /** @private Frequency of checking the distance from the player to the destination point **/
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1", ForceUnits = "s"))
    float FrequencyCheckDist{1.0f};

    /** @private Timer to check the distance of the player to the destination point **/
    FTimerHandle CheckDistToPosTimerHandle;

#pragma endregion
};
