architecture "x86_64"
language "C++"
cppdialect "C++latest"
kind "StaticLib"
files {
	"%{prj.name}/**.h",
	"%{prj.name}/**.hpp",
	"%{prj.name}/**.cpp",
	"%{prj.name}/**.hlsl",
	"%{prj.name}/**.hlsli",
	"%{prj.name}/**.hint"
}
includedirs { "%{prj.name}" }
libdirs { "../lib/**" }
pchheader "pch.h"
pchsource "%{prj.name}/pch.cpp"
defines { "WIN32_LEAN_AND_MEAN", "NOMINMAX" }
flags { "MultiProcessorCompile" }
objdir "../obj"
targetdir "../lib/%{prj.name}"
targetsuffix "_%{cfg.buildcfg}"
disablewarnings { "4996", "26800" }

filter { "files:**.hlsl" }
	shadermodel "5.0"
	shaderobjectfileoutput "%{wks.location}/bin/shader/%{file.basename}.cso"

filter { "files:**PS.hlsl" }
	shadertype "Pixel"

filter { "files:**VS.hlsl" }
	shadertype "Vertex"

filter {}


workspace "Platformer"
	location ".."
	startproject "App"
	configurations { "Debug", "Release" }

	filter "configurations:Debug"
		symbols "FastLink"
		defines { "_DEBUG" }

	filter "configurations:Release"
		symbols "FastLink"
		defines { "NDEBUG" }
		optimize "On"

project "External"
	location "%{prj.name}"
	flags { "NoPCH" }

project "Engine"
	location "%{prj.name}"
	includedirs { "External" }
	links { "External" }
	defines {
		"CBUFFER_SLOT_CAMERA=0",
		"CBUFFER_SLOT_SPRITE=1",
		"CBUFFER_SLOT_LINESTRIP=2",
		"CBUFFER_SLOT_TRANSITION=3",
		"CBUFFER_SLOT_POSTPROCESS=4",

		"TEXTURE_SLOT_SPRITE=0",
		"TEXTURE_SLOT_TRANSITION_OLD=1",
		"TEXTURE_SLOT_TRANSITION_NEW=2",
		"TEXTURE_SLOT_POSTPROCESS=3",
	}

project "Game"
	location "%{prj.name}"
	includedirs { "External", "Engine" }
	links { "External", "Engine" }
	defines { "LDTK_CELL_SIZE=16", "MAX_PLAYER_HEALTH=8" }
	
project "App"
	kind "WindowedApp"
	location "%{prj.name}"
	includedirs { "External", "Engine", "Game" }
	links { "External", "Engine", "Game" }
	targetdir "../bin"

	links { "d3d11" }

	filter "configurations:Debug"
		links { "fmodL_vc", "fmodstudioL_vc" }

	filter "configurations:Release"
		links { "fmod_vc", "fmodstudio_vc" }
