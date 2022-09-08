// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestRPGSystem/Components/QuestManagerDataTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QuestLibrary.generated.h"

class UTaskBase;
class UListTaskBase;
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
     * @private Creates a class of type UListTaskBase in memory
     * @param1 UObject* Owner
     * @param2 FSoftObjectPath PathToListTask
     * @return UListTaskBase*
     **/
    static UListTaskBase* CreateListTaskFromPath(UObject* Owner, const FSoftObjectPath& PathListTask);

    /**
     * @private Fill data info tasks
     * @param1 UListTaskBase*
     * @return TArray<FDataInfoTask>
     **/
    static TArray<FDataInfoTask> FillDataInfoTasksFromListTask(UListTaskBase* ListTask);

    /**
     * @public Check client
     * @param1 AActor*
     * @return bool
     **/
    UFUNCTION(BlueprintPure, Category = "QuestLibrary")
    static bool CheckIsClient(const AActor* InActor);
};
