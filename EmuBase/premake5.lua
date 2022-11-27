project "EmuBase"
    language "C++"
    cppdialect "C++20"
	staticruntime "on"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)
	
	pchheader "glpch.h"
	pchsource "src/glpch.cpp"

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
        "dependencies/stb_image/**.cpp",
        "dependencies/stb_image/**.cpp",
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{IncludeDir.EmuBase}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.spdlog}",
    }

	links
	{
		"glad",
		"glfw",
		"imgui",
		"spdlog",
	}
	
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