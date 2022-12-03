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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bEnableTimerTask{false};

    // Timer in sec
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1.0", ClampMax = "3540.0", ForceUnits = "s",
        EditCondition = "bEnableTimerTask"))
    float TimerTask{10.0f};

    // Enable timer
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bEnableCheckAbortTask{true};

    // Timer in sec
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1.0", ClampMax = "3540.0", ForceUnits = "s",
        EditCondition = "bEnableCheckAbortTask"))
    float CallFrequencyAbortTask{5.0f};
    
    // Enable on subtitles?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bAddUniqueTag{false};

    // Enable on subtitles?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bAddDialogTag"))
    FString NameUniqueTag{""};
};
