// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AAAMechanics : ModuleRules
{
	public AAAMechanics(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",
			"UMG",
			 });
	}
}
