project "Graphics"
    language "C++"
    cppdialect "C++20"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)
	
	pchheader "glpch.h"
	pchsource "src/grpch.cpp"

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{IncludeDir.Graphics}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glfw}",
    }

	links
	{
		"glad",
		"glfw",
	}
	
    filter "system:windows"
        kind "StaticLib"
        staticruntime "off"
        systemversion "latest"
		
	filter "system:linux"
        kind "SharedLib"
        staticruntime "off"
        pic "On"
        systemversion "latest"

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
		runtime "Release"
        optimize "on"