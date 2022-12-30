
workspace "JGGM"
	architecture "x64"
	startproject "JGGMEditor"

	configurations
	{
		"Debug",
		"Release"
	}

	filter "system:windows"
	systemversion "latest"

ProjDir = "%{wks.location}/"

include "Scripts/CommonPremake.lua"

group "ThirdParty"
	include "Source/ThirdParty/Imgui"
group ""

include "Scripts/JGGMEditorPremake.lua"


