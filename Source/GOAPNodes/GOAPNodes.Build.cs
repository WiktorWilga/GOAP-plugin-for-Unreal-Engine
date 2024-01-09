// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

using UnrealBuildTool;

public class GOAPNodes : ModuleRules
{
	public GOAPNodes(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GameplayTags",
				"UnrealEd",
				"BlueprintGraph",
				"GraphEditor",
				"PropertyEditor",
				"SlateCore",
				"Slate",
				"EditorStyle",
				"Kismet",
				"KismetCompiler",
				"DeveloperSettings"
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"GOAP"
			}
			);
	}
}
