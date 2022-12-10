project "EmuCpp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
	staticruntime "on"
		
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
        "%{IncludeDir.EmuBase}",
        "%{IncludeDir.Gameboy}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.magic_enum}",
        "%{IncludeDir.pfd}",
        "%{IncludeDir.spdlog}",
    }

	links
	{
        "EmuBase",
        "Gameboy",
	}

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
		runtime "Release"
        optimize "on"