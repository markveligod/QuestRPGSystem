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

// List task type
UENUM()
enum class ETypeListTask: uint8
{
    Visible,
    Hidden
};

// Settings run hidden list task
UENUM()
enum class ERunHiddenListTask: uint8
{
    AddToQuest,
    InitToRunListTask
};

// Action hidden to init
UENUM()
enum class EActionHiddenListTask: uint8
{
    Nothing,
    CompleteQuest,
    ForceTransferToNextBlock,
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
