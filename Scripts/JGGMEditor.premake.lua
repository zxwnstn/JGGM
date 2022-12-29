outputdir = "%{cfg.architecture}-%{cfg.buildcfg}"

project "JGGMEngine"
	location "Intermediate/ProjectFiles"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("Intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/Runtime/**.h",
		"Source/Runtime/**.cpp",
    }
	
	includedirs
	{
		"%{prj.name}/Source/Runtime"
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