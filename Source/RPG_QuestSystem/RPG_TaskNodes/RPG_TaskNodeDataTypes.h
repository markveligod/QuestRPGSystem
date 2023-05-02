/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "RPG_TaskNodeDataTypes.generated.h"

/** @struct Minimum data set for debug information **/
USTRUCT(BlueprintType)
struct FRPG_DrawDebugInfoMinimal
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-1.0", ForceUnits = "s"))
    float TimeToDisplay{10.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor DisplayColor{FColor::Orange};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bNewerOnTop{true};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D TextScale{FVector2D::UnitVector};
};

/** @enum Way's to search for an entity in the world **/
UENUM(BlueprintType)
enum class ERPG_WaySearchEntity : uint8
{
    ByTag,
    ByClass,
    ByObject
};
