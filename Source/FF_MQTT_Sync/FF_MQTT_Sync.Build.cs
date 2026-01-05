using System;
using System.IO;
using UnrealBuildTool;

public class FF_MQTT_Sync : ModuleRules
{
	public FF_MQTT_Sync(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        CppCompileWarningSettings.UndefinedIdentifierWarningLevel = WarningLevel.Off;
        bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "paho_c_sync",
            });
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Json",
                "JsonUtilities",
                "JsonBlueprintUtilities",
            });
	}
}
