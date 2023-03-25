project "EmuBase"
    language "C++"
    cppdialect "C++20"
	staticruntime "on"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
        "src/**.ixx",
    }

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{IncludeDir.glm}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.magic_enum}",
        "%{IncludeDir.pfd}",
        "%{IncludeDir.spdlog}",
    }

	links
	{
		"glad",
		"glfw",
		"imgui",
		"spdlog",
	}
    
    filter { "files:**.ixx" }
        compileas "Module"
        
    filter { "files:**.ixxp" }
        compileas "ModulePartition"

	filter { "files:**.h" }
        compileas "HeaderUnit"
	
    filter "system:windows"
        kind "StaticLib"
        systemversion "latest"
		
	filter "system:linux"
        kind "SharedLib"
        pic "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "EMU_DEBUG" }
		runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines { "EMU_RELEASE" }
		runtime "Release"
        optimize "on"