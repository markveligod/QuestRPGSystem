#pragma once

#include "CoreMinimal.h"
#include "ListTaskDataTypes.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateListTaskSignature, class UListTaskBase*);

// List task start type
UENUM(BlueprintType)
enum class ETypeRunListTask: uint8
{
    StepByStep,
    AllSameTime,
};

// List task status type
UENUM()
enum class EStatusListTask: uint8
{
    None,
    Init,
    Run,
    Complete,
};

