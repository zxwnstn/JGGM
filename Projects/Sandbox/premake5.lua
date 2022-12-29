workspace "Sandbox"
	architecture "x64"
	startproject "JGGMEngine"

	configurations
	{
		"Debug",
		"Release"
	}

include "../../Scripts/JGGMLibrary.premake.lua"
include "Scripts/Sandbox.premake.lua"
