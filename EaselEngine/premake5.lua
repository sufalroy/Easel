project "EaselEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"


	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "Precompiled.h"
	pchsource "src/Precompiled.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	defines
	{
		"EASEL_ENGINE",
		"EASEL_DYNAMIC",
		"EASEL_ROOT_DIR=" .. root_dir,
	}

	includedirs
	{
		"src/",
		"3rdparty/spdlog/include",
		"%{IncludeDir.stb}",
	}

	links
	{

	}


	filter "system:windows"
		systemversion "latest"

		defines 
		{
			"EASEL_PLATFORM_WINDOWS",
			"_CRT_SECURE_NO_WARNINGS",
		}

		buildoptions
		{
			"/MP", "/bigobj"
		}

		disablewarnings { 4307 }

		characterset ("MBCS")

		filter 'files:3rdparty/**.cpp'
			flags  { 'NoPCH' }
		filter 'files:3rdparty/**.c'
			flags  { 'NoPCH' }

	filter "configurations:Debug" 
		defines {"EASEL_DEBUG", "_DEBUG"}
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
