// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestObject.generated.h"

class UQuestNode;
class UTaskBase;

UENUM()
enum class ETypeNode: uint8
{
    Root,
    Usual,
};

USTRUCT()
struct FTaskNode
{
    GENERATED_BODY()

    ETypeNode TypeNode{ETypeNode::Usual};

    int32 ID{INDEX_NONE};

    UPROPERTY(Instanced)
    UTaskBase* Task{nullptr};

    TMap<FString, int32> ConnectNextNode;
};

/**
 * 
 */
UCLASS(BlueprintType)
class QUESTRPGSYSTEM_API UQuestObject : public UObject
{
    GENERATED_BODY()

public:

    /** @public Init quest**/
    virtual bool InitQuest() { return true; }

    /** @public Run quest**/
    virtual bool RunQuest() { return true; }

    /** @public Get All nodes **/
    const TArray<FTaskNode>& GetAllNodes() { return AllNodes; }

    /** @public Save state quest **/
    virtual FString SaveQuest() { return FString(); }

#pragma region Editor
    
protected:


#if WITH_EDITOR
    virtual void PostInitProperties() override;
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
    static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
#endif

#pragma endregion
    
private:

    UPROPERTY()
    UEdGraph* QuestGraph;
    
    UPROPERTY()
    TArray<FTaskNode> AllNodes;
};
