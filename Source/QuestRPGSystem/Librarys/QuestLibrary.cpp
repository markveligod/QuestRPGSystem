// Fill out your copyright notice in the Description page of Project Settings.


#include "Librarys/QuestLibrary.h"

void UQuestLibrary::Print_Log(const ELogVerb LogVerb, const FString Text, const int Line, const char* Function)
{
    if (LogVerb == ELogVerb::Display)
    {
        UE_LOG(LogQuestSystem, Display, TEXT("[%s][%d] TEXT:[%s]"), ANSI_TO_TCHAR(Function), Line, *Text);
    }
    else if (LogVerb == ELogVerb::Warning)
    {
        UE_LOG(LogQuestSystem, Warning, TEXT("[%s][%d] TEXT:[%s]"), ANSI_TO_TCHAR(Function), Line, *Text);
    }
    else if (LogVerb == ELogVerb::Error)
    {
        UE_LOG(LogQuestSystem, Error, TEXT("[%s][%d] TEXT:[%s]"), ANSI_TO_TCHAR(Function), Line, *Text);
    }
}

bool UQuestLibrary::CheckedCondition(const bool bCondition, const FString Text, const int Line, const char* Function)
{
    if (!bCondition)
    {
        Print_Log(ELogVerb::Error, Text, Line, Function);
        return false;
    }
    return true;
}
