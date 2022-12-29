
workspace "Sandbox"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}
	
	filter "system:windows"
	systemversion "latest"

EngineDir = "../"
ProjDir   = "%{wks.location}/"
include "../../Scripts/CommonPremake.lua"
include "../../Scripts/JGGMLibraryPremake.lua"

EngineDir = "../../../"
include "Scripts/Sandboxpremake.lua"
