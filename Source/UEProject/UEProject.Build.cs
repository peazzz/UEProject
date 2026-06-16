// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UEProject : ModuleRules
{
	public UEProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"UEProject",
			"UEProject/Variant_Platforming",
			"UEProject/Variant_Platforming/Animation",
			"UEProject/Variant_Combat",
			"UEProject/Variant_Combat/AI",
			"UEProject/Variant_Combat/Animation",
			"UEProject/Variant_Combat/Gameplay",
			"UEProject/Variant_Combat/Interfaces",
			"UEProject/Variant_Combat/UI",
			"UEProject/Variant_SideScrolling",
			"UEProject/Variant_SideScrolling/AI",
			"UEProject/Variant_SideScrolling/Gameplay",
			"UEProject/Variant_SideScrolling/Interfaces",
			"UEProject/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
