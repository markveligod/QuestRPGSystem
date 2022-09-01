// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QuestLibrary.generated.h"

// Custom verb to logging
UENUM()
enum class ELogVerb: uint8
{
    Display,
    Warning,
    Error
};

DEFINE_LOG_CATEGORY_STATIC(LogQuestSystem, All, All);
#define LOG_QUEST(LogVerb, Text) UQuestLibrary::Print_Log(LogVerb, Text, __LINE__, __FUNCTION__)
#define CHECK_COND(Cond, Text) UQuestLibrary::CheckedCondition(Cond, Text, __LINE__, __FUNCTION__)

/**
 * @class Function quest library
 */
UCLASS()
class QUESTRPGSYSTEM_API UQuestLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /**
     * @public Write a log
     **/
    static void Print_Log(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function);

    /**
     * @public Check the condition
     * @return bool
     **/
    static bool CheckedCondition(const bool bCondition, const FString Text, const int Line, const char* Function);

};
