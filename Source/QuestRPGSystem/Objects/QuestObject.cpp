// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/QuestObject.h"


#pragma region UObjectInterface

#if WITH_EDITOR

void UQuestObject::PostInitProperties()
{
    
}

void UQuestObject::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    
}

void UQuestObject::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
    UQuestObject* This = CastChecked<UQuestObject>(InThis);
    Collector.AddReferencedObject(This->QuestGraph, This);
    Super::AddReferencedObjects(InThis, Collector);
}

#endif

#pragma endregion
