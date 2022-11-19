#pragma once

#include "CoreMinimal.h"
#include "TaskDataTypes.generated.h"

/**
 * Call for update task status
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateTaskSignature, class UTaskBase*);

UENUM()
enum class EStatusTask: uint8
{
    NoneInit,
    Init,
    Run,
    Complete
};

/**
 * @struct Task-specific settings
 **/
USTRUCT(BlueprintType)
struct FTaskSpecificSettings
{
    GENERATED_BODY()

    // Enable timer
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableTimerTask{false};

    // Timer in sec
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1.0", ClampMax = "3540.0", ForceUnits = "s",
        EditCondition = "bEnableTimerTask", EditConditionHides))
    float TimerTask{10.0f};

    // Enable on subtitles?
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bAddUniqueTag{false};

    // Enable on subtitles?
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bAddDialogTag"))
    FString NameUniqueTag{""};
};
