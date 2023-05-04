/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RPG_QuestHandleSubsystem.generated.h"

/**
 *
 */
UCLASS()
class RPG_QUESTSYSTEM_API URPG_QuestHandleSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

#pragma region DefaultInterface
    
public:

    virtual void BeginPlay();

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
    
};
