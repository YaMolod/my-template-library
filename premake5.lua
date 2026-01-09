workspace "MTL"
	architecture "x64"
	startproject "GoogleTests"
	
	configurations
	{
		"Debug",
		"Release",
	}
	
project "MTL"
	location "MTL"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/%{cfg.buildcfg}/%{prj.name}")
	objdir ("bin-intermediates/%{cfg.buildcfg}/%{prj.name}")

	files
	{
		"%{prj.name}/%{prj.name}/**.hpp"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "GoogleTests"
	location "GoogleTests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir ("bin/%{cfg.buildcfg}/%{prj.name}")
	objdir ("bin-intermediates/%{cfg.buildcfg}/%{prj.name}")

	files
	{
		"%{prj.name}/src/gtest_main.cc",
		"%{prj.name}/src/gtest-all.cc",
		"%{prj.name}/include/**.h",
		"%{prj.name}/tests/**.cpp",
		"%{prj.name}/tests/**.h"
	}

	includedirs
	{
		"MTL",
		"%{prj.name}",
		"%{prj.name}/include"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		