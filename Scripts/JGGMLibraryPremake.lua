
project "JGGMEngine"
	location (ProjDir .. ProjectFilesDir)
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir (ProjDir .. OutputDir)
	objdir (ProjDir .. BinIntDir)

	files
	{
		EngineDir .. "Source/Runtime/**.h",
		EngineDir .. "Source/Runtime/**.cpp"
    }
	
	includedirs
	{
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

	