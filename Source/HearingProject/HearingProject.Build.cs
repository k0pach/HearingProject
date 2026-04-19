// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HearingProject : ModuleRules
{
	public HearingProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayTasks",
			"NavigationSystem",
			"StructUtils",
			"PropertyBindingUtils",
			"StructUtilsEngine",
			"AIModule",
			"GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"HearingProject",
			"HearingProject/Variant_Platforming",
			"HearingProject/Variant_Platforming/Animation",
			"HearingProject/Variant_Combat",
			"HearingProject/Variant_Combat/AI",
			"HearingProject/Variant_Combat/Animation",
			"HearingProject/Variant_Combat/Gameplay",
			"HearingProject/Variant_Combat/Interfaces",
			"HearingProject/Variant_Combat/UI",
			"HearingProject/Variant_SideScrolling",
			"HearingProject/Variant_SideScrolling/AI",
			"HearingProject/Variant_SideScrolling/Gameplay",
			"HearingProject/Variant_SideScrolling/Interfaces",
			"HearingProject/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
