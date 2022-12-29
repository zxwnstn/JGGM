
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
include "Scripts/JGGMEditorPremake.lua"
