// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPG_QuestSystem : ModuleRules
{
    public RPG_QuestSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] {
            // ... add public include paths required here ...
        });

        PrivateIncludePaths.AddRange(new string[] {// ... add other private include paths required here ...
            "RPG_QuestSystem"
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            // ... add other public dependencies that you statically link with here ...
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "CoreUObject", "Engine", "Slate", "SlateCore", "JsonUtilities", "DeveloperSettings",
            // ... add private dependencies that you statically link with here ...
        });

        DynamicallyLoadedModuleNames.AddRange(new string[] {
            // ... add any modules that your module loads dynamically here ...
        });
    }
}