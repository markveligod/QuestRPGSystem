// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestManagerDataTypes.h"
#include "QuestManagerBase.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class QUESTRPGSYSTEM_API UQuestManagerBase : public UActorComponent
{
    GENERATED_BODY()

#pragma region Default
    
public:

    // Sets default values for this component's properties
    UQuestManagerBase();

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

#pragma endregion

#pragma region DataQuestManager

protected:

    // @protected Current array data quest
    UPROPERTY()
    TArray<FDataQuest> ArrayDataQuest;
    
    // @protected Data quest table
    UPROPERTY(EditAnywhere, Category = "Data Quest Manager")
    UDataTable* DataQuestTable;

#pragma endregion
    
};
