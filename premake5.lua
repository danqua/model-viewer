workspace "ModelViewer"
	architecture "x86_64"
	configurations { "Debug", "Release" }
	targetdir "bin/%{cfg.buildcfg}"
	debugdir "data"

project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++98"
	
	files {
		"extern/imgui-docking/imgui*.cpp",
		"extern/imgui-docking/backends/imgui_impl_sdl.cpp",
		"extern/imgui-docking/backends/imgui_impl_opengl3.cpp",
	}

	includedirs {
		"extern/imgui-docking/",
		"extern/SDL2-2.0.20/include"
	}

project "glad"
	kind "StaticLib"
	language "C++"
	cppdialect "C++98"

	files {
		"extern/glad/src/glad.c"
	}
	
	includedirs {
		"extern/glad/include"
	}

project "ModelViewer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	files {
		"code/**.h",
		"code/**.cpp"
	}

	includedirs {
		"code/",
		"extern/glad/include",
		"extern/glm",
		"extern/imgui-docking/",
		"extern/imgui-docking/backends",
		"extern/SDL2-2.0.20/include",
		"extern/tinyobjloader",
		"extern/stb",
		"extern/assimp/include",
		"extern/assimp/build/include"
	}

	libdirs {
		"extern/SDL2-2.0.20/lib/x64",
		"extern/assimp/build/lib/Release"
	}

	links {
		"ImGui",
		"glad",
		"SDL2",
		"SDL2main",
		"assimp-vc142-mt"
	}

	
	flags { "NoImportLib" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"