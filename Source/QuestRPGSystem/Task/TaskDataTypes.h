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

//
UENUM(BlueprintType)
enum class EColorSubtitle : uint8
{
    Blue,
    Red,
    Yellow,
    Black
};

/**
 * @struct All the information you need to play subtitles
 **/
USTRUCT(BlueprintType)
struct FInfoSubtitles
{
    GENERATED_BODY()

    // The name of the subtitles to display
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText InName{};

    // Text played by the speaker
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
    FText InText{};

    // Type of widget to display
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EColorSubtitle InTypeSub{EColorSubtitle::Blue};

    // Playback of the speaker's voice
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowedClasses = "SoundWave,SoundCue"))
    FSoftObjectPath InSound{};

    // Fine-tuning speaker sounds
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowedClasses = "SoundAttenuation"))
    FSoftObjectPath InAttenuation{};

    // Fine-tuning speaker sounds
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowedClasses = "SoundConcurrency"))
    FSoftObjectPath InConcurrency{};

    // The playback time of the current subtitles
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceUnits = "s", ClampMin = "0.1", ClampMax = "1000.0"))
    float DelayTime{0.1f};
};

/**
 * @struct Task-specific settings
 **/
USTRUCT(BlueprintType)
struct FTaskSpecificSettings
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FVector> ArrPointPositionForMap;
    
    // Allow for adding additional text to the description
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableHaveSubDescription{false};
    
    // Whether to hide the description of the task of the right side
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableHideTaskDescription{false};
    
    // Enable timer
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableTimerTask{false};

    // Timer in sec
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1.0", ClampMax = "3540.0", ForceUnits = "s",
        EditCondition = "bEnableTimerTask", EditConditionHides))
    float TimerTask{10.0f};

    // Deviation of the current marker from the original position
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ForceUnits = "cm", ClampMin = "-15000.0", ClampMax = "15000.0"))
    float TargetMarkDeviation{0.0f};

    // Enable general zone
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableZone{false};

    // The central point of the zone
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bEnableZone", EditConditionHides))
    FVector CenterZone{FVector::ZeroVector};

    // Radius zone
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ForceUnits = "cm", EditCondition = "bEnableZone",
        EditConditionHides, ClampMin = "100.0", ClampMax = "200000.0"))
    float RadiusZone{2000.0f};

    // Enable compass search (Hot - Cold)
    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bEnableZone", EditConditionHides))
    bool bEnableCompassSearch{false};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Units = "cm", EditCondition = "bEnableCompassSearch && bEnableZone",
        EditConditionHides, ClampMin = "500.0", ClampMax = "20000"))
    float RadiusSearch{2000.0f};

    // Enable on subtitles?
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bAddDialogTag{false};

    // Enable on subtitles?
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bAddDialogTag", DisplayName = "Сохранить Тэг",
        ToolTip = "По завершению задачи будет добавлен тэг в квестовый менеджер"))
    FString AddTag{""};

    // Enable on subtitles?
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableSubtitles{false};

    // Delay before starting all dialogs
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ForceUnits = "s", EditCondition = "bEnableSubtitles", EditConditionHides,
        ClampMin = "0.1", ClampMax = "3540.0"))
    float DelayBeforeStartDialog{0.1f};

    // All information about subtitles
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bEnableSubtitles", EditConditionHides, TitleProperty = "InName"))
    TArray<FInfoSubtitles> ArrayInfoSubtitles;
};
