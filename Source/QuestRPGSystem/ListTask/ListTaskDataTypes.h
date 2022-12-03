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
    TransferListTask,
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

USTRUCT(BlueprintType)
struct FDataTransferToNextBlock
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere)
    int32 Index{0};

    UPROPERTY(EditAnywhere, meta = (MetaClass = "ListTaskBase"))
    FSoftClassPath NextPathBlock;

    FDataTransferToNextBlock() {}
    FDataTransferToNextBlock(const int32 NewIndex): Index(NewIndex) {}

    bool operator==(const FDataTransferToNextBlock& Other) const
    {
        return this->Index == Other.Index;
    }
};
