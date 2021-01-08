project "Easel"
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
		"3rdparty/glm/glm/**.hpp",
		"3rdparty/glm/glm/**.inl",
		"3rdparty/tracy/TracyClient.cpp",
	}

	includedirs
	{
		"src/"
	}

	sysincludedirs
	{
		"3rdparty/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.cereal}",
		"%{IncludeDir.tracy}",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	defines
	{
		"EASEL_ENGINE",
		"EASEL_DYNAMIC",
		"EASEL_ROOT_DIR=" .. root_dir,
		"EASEL_PROFILE",
		"TRACY_ENABLE"
	}


	filter "system:windows"
		systemversion "latest"

		defines 
		{
			"EASEL_PLATFORM_WINDOWS",
			"EASEL_RENDER_API_OPENGL",
			"EASEL_USE_GLFW_WINDOWS",
			"WIN32_LEAN_AND_MEAN",
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
			"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
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
