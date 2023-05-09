/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "RPG_QuestHandleSubsystem.generated.h"

/**
 * @class Subsystem for wider manipulation of quest parameters
 */
UCLASS(DisplayName = "QuestHandleSubsystem")
class RPG_QUESTSYSTEM_API URPG_QuestHandleSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

#pragma region DefaultInterface

public:
    // FTickableGameObject implementation Begin
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    // FTickableGameObject implementation End

    // USubsystem implementation Begin
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // USubsystem implementation End

#pragma endregion

#pragma region Signature

private:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBroadcastQuestTagSignature, APlayerController*, Owner, FGameplayTag, Tag);

public:

    UFUNCTION(BlueprintCallable)
    void ActionBroadcastQuestTag(APlayerController* PC, FGameplayTag Tag) { OnBroadcastQuestTag.Broadcast(PC, Tag); }
    
    /** @public Signature for the event event on receipt of the tag **/
    UPROPERTY(BlueprintAssignable)
    FBroadcastQuestTagSignature OnBroadcastQuestTag;

#pragma endregion
};
