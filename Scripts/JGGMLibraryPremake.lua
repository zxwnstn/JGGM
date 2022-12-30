group "ThirdParty"
	include (EngineDir .. "Source/ThirdParty/Imgui")
group ""

group "JGGM"
project "JGGMEngine"
	location (ProjDir .. ProjectFilesDir)
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	
	targetdir (ProjDir .. OutputDir)
	objdir (ProjDir .. BinIntDir)

	files
	{
		EngineDir .. "Source/Runtime/**.h",
		EngineDir .. "Source/Runtime/**.cpp"
    }
	
	includedirs
	{
		EngineDir .. "Source/Runtime",
		EngineDir .. "Source"
	}

	links
	{
		"Imgui"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "JGGM_DEBUG"
		runtime "Debug"
		symbols "on"

		
	filter "configurations:Release"
		defines "JGGM_RELEASE"
		runtime "Release"
		optimize "on"
group ""
	