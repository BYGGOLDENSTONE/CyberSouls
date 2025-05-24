// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class cybersouls : ModuleRules
{
	public cybersouls(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"UMG",
			"AIModule",
			"NavigationSystem"
		});
		
		PublicIncludePaths.AddRange(new string[] {
			"cybersouls/Public",
			"cybersouls/Public/AI",
			"cybersouls/Public/Abilities",
			"cybersouls/Public/Attributes", 
			"cybersouls/Public/Character",
			"cybersouls/Public/Combat",
			"cybersouls/Public/Enemy",
			"cybersouls/Public/Game",
			"cybersouls/Public/Input",
			"cybersouls/Public/Player",
			"cybersouls/Public/UI"
		});
	}
}
