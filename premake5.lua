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
IncludeDir["EmuBase"] 	= "%{wks.location}/EmuBase/src"
IncludeDir["AppCore"] 	= "%{wks.location}/AppCore/src"
IncludeDir["Gameboy"] 	= "%{wks.location}/Gameboy/src"
IncludeDir["glad"] 	    = "%{wks.location}/EmuBase/dependencies/glad/include"
IncludeDir["glfw"] 	    = "%{wks.location}/EmuBase/dependencies/glfw/include"
IncludeDir["glm"] 	    = "%{wks.location}/EmuBase/dependencies/glm"
IncludeDir["imgui"] 	= "%{wks.location}/EmuBase/dependencies/imgui"
IncludeDir["spdlog"] 	= "%{wks.location}/EmuBase/dependencies/spdlog/include"

group "Dependencies"
	include "EmuBase/dependencies/glad"
	include "EmuBase/dependencies/glfw"
	include "EmuBase/dependencies/imgui"
	include "EmuBase/dependencies/spdlog"
group ""

group "Core"
    include "EmuBase"
group ""

group "Emulators"
    include "Gameboy"
group ""

group "Application"
    include "EmuCpp"
group ""