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


group "Dependencies"
	include "vendor/premake"
group ""

include "EaselEngine"
include "Sandbox"
