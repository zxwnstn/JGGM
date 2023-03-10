
project "JGGMEditor"
	location (ProjDir .. ProjectFilesDir)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	
	targetdir (ProjDir .. OutputDir)
	objdir (ProjDir .. BinIntDir)
	
	pchheader ("JGGMPch.h")
	pchsource (ProjDir .. "Source/Runtime/JGGMPch.cpp")

	files
	{
		ProjDir .. "Source/Runtime/**.h",
		ProjDir .. "Source/Runtime/**.cpp"
    }
	
	includedirs
	{
		ProjDir .. "Source/Runtime",
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

	

