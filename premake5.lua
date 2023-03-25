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
IncludeDir["EmuCpp"] 	  = "%{wks.location}/EmuCpp/src"
IncludeDir["glad"] 	      = "%{wks.location}/EmuBase/dependencies/glad/include"
IncludeDir["glfw"] 	      = "%{wks.location}/EmuBase/dependencies/glfw/include"
IncludeDir["glm"] 	      = "%{wks.location}/EmuBase/dependencies/glm"
IncludeDir["imgui"] 	  = "%{wks.location}/EmuBase/dependencies/imgui"
IncludeDir["magic_enum"]  = "%{wks.location}/EmuBase/dependencies/magic_enum"
IncludeDir["pfd"] 	      = "%{wks.location}/EmuBase/dependencies/pfd"
IncludeDir["spdlog"] 	  = "%{wks.location}/EmuBase/dependencies/spdlog/include"

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