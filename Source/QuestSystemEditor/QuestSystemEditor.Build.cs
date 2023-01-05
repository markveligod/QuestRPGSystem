
namespace UnrealBuildTool.Rules
{
	public class QuestSystemEditor : ModuleRules
	{
		public QuestSystemEditor(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
				}
			);
				
		
			PrivateIncludePaths.AddRange(
				new string[] {
					// ... add other private include paths required here ...
				}
			);

			
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"EditorStyle",
					"Engine",
					"InputCore",
					"LevelEditor",
					"Slate",
					"AssetTools",
					"KismetWidgets",
					"WorkspaceMenuStructure",
					"Projects",
					"GraphEditor",
					"AnimGraph"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{					
					"PropertyEditor",
					"SlateCore",
					"ApplicationCore",
					"UnrealEd",
					"Json",
					"JsonUtilities",
					"QuestRPGSystem"
				}
			);

			PrivateIncludePathModuleNames.AddRange(
				new string[] 
				{
					"MainFrame",
					"Settings"
				}
			);
		}
	}
}
