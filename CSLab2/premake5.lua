-- Main premake
include "Dependencies.lua"

workspace "CSLab2"
    architecture "x64"
    startproject "CSLab2"
    configurations { "Debug", "Release"}
    platforms {"Win64"}
    buildoptions { "/MP "}

    location "..\\"

    outputdir = "%{cfg.platform}/%{cfg.buildcfg}"
    bin_out = "%{wks.location}/CSLab2/Binaries/Application/" .. outputdir .. ""
    int_out = "%{wks.location}/CSLab2/Intermediate/Application/" .. outputdir .. ""
    tp_bin_out = "%{wks.location}/CSLab2/Binaries/ThirdParty/" .. outputdir .. ""
    tp_int_out = "%{wks.location}/CSLab2/Intermediate/ThirdParty/" .. outputdir .. ""

    debugdir "%{wks.location}/CSLab2"


group "Dependencies"
	include "ThirdParty/GLFW"
	include "ThirdParty/Glad"
	include "ThirdParty/imgui"
group ""

group "Application"
    include "Source"
group ""

