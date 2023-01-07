// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestObject.generated.h"

class UListTaskBase;
/**
 * 
 */
UCLASS(BlueprintType)
class QUESTRPGSYSTEM_API UQuestObject : public UObject
{
	GENERATED_BODY()

public:

    TArray<UListTaskBase*> AllNodes;
};
