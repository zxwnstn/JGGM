
project "Sandbox"
	location (ProjDir .. ProjectFilesDir)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	filter "configurations:Debug"
		defines "JGGM_DEBUG"
		runtime "Debug"
		symbols "on"
		targetdir (ProjDir .. OutputDir)
		objdir (ProjDir .. BinIntDir)

	filter "configurations:Release"
		defines "JGGM_RELEASE"
		runtime "Release"
		optimize "on"
		targetdir (ProjDir .. OutputDir)
		objdir (ProjDir .. BinIntDir)

	files
	{
		ProjDir .. "Source/**.h",
		ProjDir .. "Source/**.cpp"
    }
	
	includedirs
	{
		ProjDir .. "Source",
		EngineDir .. "Source"
	}

    links
	{
		"JGGMEngine"
	}

