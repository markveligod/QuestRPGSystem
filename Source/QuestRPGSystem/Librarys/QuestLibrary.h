// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestRPGSystem/Components/QuestManagerDataTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QuestLibrary.generated.h"

class UTaskBase;
class UListTaskBase;

// Custom verb to logging
enum EQuestLogVerb
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
    static void Print_Log(const TEnumAsByte<EQuestLogVerb> LogVerb, const FString Text, const int Line, const char* Function);

    /**
     * @public Check the condition
     **/
    static bool CheckedCondition(const bool bCondition, const FString Text, const int Line, const char* Function);

    /**
     * @public Template for string name from ENum
     * @example GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
     **/
    template<typename TEnum>
    static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
    {
        const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
        if (!enumPtr) return FString("Invalid");
        return enumPtr->GetDisplayNameTextByIndex(static_cast<int32>(Value)).ToString();
    }

    /**
     * @public Get String by timer in sec
     **/
    static FString GetStringTimeFromSecond(float Seconds);

    /**
     * @public Creates a class of type UListTaskBase in memory
     **/
    static UListTaskBase* LoadListTaskFromPath(UObject* Owner, const FSoftObjectPath& PathListTask);

    /**
     * @public Creates a class of type UListTaskBase in memory
     **/
    static void UnLoadListTaskFromPath(const FSoftObjectPath& PathListTask);

    /**
     * @public Fill data info tasks
     **/
    static TArray<FDataInfoTask> FillDataInfoTasksFromListTask(UListTaskBase* ListTask);

    /**
     * @public Check client
     **/
    UFUNCTION(BlueprintPure, Category = "QuestLibrary")
    static bool CheckIsClient(const AActor* InActor);

    /**
     * @public Get net mode to string 
     **/
    UFUNCTION(BlueprintPure, Category = "QuestLibrary")
    static FString GetNetModeToString(const AActor* Actor);
};
