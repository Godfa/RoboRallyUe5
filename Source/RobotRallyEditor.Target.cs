using UnrealBuildTool;
using System.Collections.Generic;

public class RobotRallyEditorTarget : TargetRules
{
	public RobotRallyEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("RobotRally");
	}
}
