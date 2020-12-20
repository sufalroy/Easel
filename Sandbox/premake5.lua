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
		"%{wks.location}/EaselEngine/3rdparty/spdlog/include",
		"%{wks.location}/EaselEngine/src",
		"%{wks.location}/EaselEngine/3rdparty",
		"%{IncludeDir.glm}",
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
			"WIN32_LEAN_AND_MEAN",
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
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