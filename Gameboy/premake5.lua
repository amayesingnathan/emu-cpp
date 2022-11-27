project "Gameboy"
    language "C++"
    cppdialect "C++20"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)
	
	pchheader "gbpch.h"
	pchsource "src/gbpch.cpp"

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
        "%{IncludeDir.EmuBase}",
        "%{IncludeDir.Gameboy}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
    }

	links
	{
        "EmuBase",
	}
	
    filter "system:windows"
        kind "StaticLib"
        staticruntime "off"
        systemversion "latest"
		
	filter "system:linux"
        kind "SharedLib"
        staticruntime "off"
        defines "GB_SHARED"
        pic "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "GB_DEBUG" }
		runtime "Debug"
        symbols "on"
    filter "configurations:Release"
        defines { "GB_RELEASE" }
		runtime "Release"
        optimize "on"