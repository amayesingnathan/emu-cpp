workspace "emu-cpp"
    startproject "EmuCpp"

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
IncludeDir["EmuCpp"] 	= "%{wks.location}/EmuCpp/src"
IncludeDir["Base"] 	    = "%{wks.location}/EmulatorBase/src"
IncludeDir["Graphics"] 	= "%{wks.location}/Graphics/src"
IncludeDir["Gameboy"] 	= "%{wks.location}/Gameboy/src"
IncludeDir["glad"] 	    = "%{wks.location}/Graphics/dependencies/glad/include"
IncludeDir["glfw"] 	    = "%{wks.location}/Graphics/dependencies/glfw/include"

group "Dependencies"
	include "Graphics/dependencies/glad"
	include "Graphics/dependencies/glfw"
group ""

group "Core"
    include "EmulatorBase"
    include "Graphics"
group ""

group "Emulators"
    include "Gameboy"
group ""

group "Application"
    include "EmuCpp"
group ""