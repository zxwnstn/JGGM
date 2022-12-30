
project "JGGMEditor"
	location (ProjDir .. ProjectFilesDir)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir (ProjDir .. OutputDir)
	objdir (ProjDir .. BinIntDir)
	
	files
	{
		ProjDir .. "Source/Runtime/**.h",
		ProjDir .. "Source/Runtime/**.cpp"
    }
	
	includedirs
	{
		ProjDir .. "Source"
	}

	links
	{
		"Imgui"
	}

	defines
	{
		"JGGM_EDITOR"
	}

	filter "configurations:Debug"
		defines "JGGM_DEBUG"
		runtime "Debug"
		symbols "on"
		
	
	filter "configurations:Release"
		defines "JGGM_RELEASE"
		runtime "Release"
		optimize "on"

	

