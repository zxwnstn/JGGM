
project "Sandbox"
	location (ProjDir .. ProjectFilesDir)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	
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
		EngineDir .. "Source/Runtime"
	}

    links
	{
		"JGGMEngine"
	}

	filter "configurations:Debug"
		defines "JGGM_DEBUG"
		runtime "Debug"
		symbols "on"


	filter "configurations:Release"
		defines "JGGM_RELEASE"
		runtime "Release"
		optimize "on"

