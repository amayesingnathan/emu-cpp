workspace "Emulators"
    startproject "EmuGB"

    configurations 
    { 
        "Debug",
        "Release"
    }
    
    platforms
    {
        "x64",
        "ARM32",
        "ARM64"
    }

	filter "platforms:x64"
		architecture "x86_64"

	filter "platforms:ARM32"
		architecture "ARM"

 	filter "configurations:ARM64"
		architecture "ARM64"

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Graphics"] 	= "%{wks.location}/Graphics/src"
IncludeDir["Gameboy"] 	= "%{wks.location}/Gameboy/src"
IncludeDir["EmuGB"] 	= "%{wks.location}/EmuGB/src"

group "Dependencies"
group ""

group "Core"
    include "Graphics"
group ""

group "Emulators"
    include "Gameboy"
group ""

group "Application"
    include "EmuGB"
group ""