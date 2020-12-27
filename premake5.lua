include "./vendor/premake/premake_customization/solution_items.lua"

root_dir = os.getcwd()

workspace "Easel"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"]  =  "%{wks.location}/Easel/3rdparty/glfw/include"
IncludeDir["Glad"]  =  "%{wks.location}/Easel/3rdparty/glad/include"
IncludeDir["ImGui"] =  "%{wks.location}/Easel/3rdparty/imgui"
IncludeDir["stb"]   =  "%{wks.location}/Easel/3rdparty/stb"
IncludeDir["glm"]   =  "%{wks.location}/Easel/3rdparty/glm"

group "Dependencies"
	include "vendor/premake"
	include "Easel/3rdparty/glfw"
	include "Easel/3rdparty/glad"
	include "Easel/3rdparty/imgui"
group ""

include "Easel"
include "Sandbox"
