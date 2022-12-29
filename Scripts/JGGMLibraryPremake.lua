
project "JGGMEngine"
	location (ProjDir .. ProjectFilesDir)
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	filter "system:windows"
		systemversion "latest"

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
		EngineDir .. "Source/**.h",
		EngineDir .. "Source/**.cpp"
    }
	
	includedirs
	{
		EngineDir .. "Source"
	}

	