project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	defines
	{

	}

	includedirs
	{
		"%{wks.location}/Easel/src",
	}

	links
	{
		"EaselEngine"
	}

	filter "system:windows"
		systemversion "latest"

		defines 
		{
			"EASEL_PLATFORM_WINDOWS",
			"_CRT_SECURE_NO_WARNINGS",
			"EASEL_ROOT_DIR="  .. root_dir
		}

		buildoptions
		{
			"/MP"
		}

		disablewarnings { 4307 }
		

	filter "configurations:Debug" 
		defines "EASEL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "EASEL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "EASEL_DIST"
		runtime "Release"
		optimize "on"