using UnrealBuildTool;

public class RPG_QuestSystemEditor : ModuleRules
{
    public RPG_QuestSystemEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "EditorStyle", "Engine", "InputCore", "LevelEditor", "Slate", "AssetTools", "KismetWidgets",
            "WorkspaceMenuStructure", "Projects", "GraphEditor", "AnimGraph" });

        PrivateDependencyModuleNames.AddRange(new string[] { "PropertyEditor", "SlateCore", "ToolMenus", "ApplicationCore", "UnrealEd", "Json", "JsonUtilities", "RPG_QuestSystem" });

        PrivateIncludePathModuleNames.AddRange(new string[] { "MainFrame", "Settings" });
    }
}