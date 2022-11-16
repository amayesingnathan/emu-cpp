project "EmuCpp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)

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
        "%{IncludeDir.EmuCpp}",
        "%{IncludeDir.Graphics}",
        "%{IncludeDir.Gameboy}",
    }

	links
	{
        "Graphics",
        "Gameboy",
	}

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
		runtime "Release"
        optimize "on"