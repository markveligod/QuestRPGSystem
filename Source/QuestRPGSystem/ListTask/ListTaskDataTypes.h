#pragma once

#include "CoreMinimal.h"
#include "ListTaskDataTypes.generated.h"

// List task start type
UENUM(BlueprintType)
enum class ETypeRunListTask: uint8
{
    StepByStep,
    AllSameTime,
    TransferListTask,
};

// List task status type
UENUM()
enum class EStatusListTask: uint8
{
    None,
    Run,
    Complete,
};
