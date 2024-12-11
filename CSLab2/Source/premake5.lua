project "CSLab2"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	staticruntime "off"

    targetdir (bin_out)
    objdir (int_out)
    targetname("CSLab2")

    location "%{wks.location}/CSLab2/Build/ProjectFiles"

    files
    {
        "%{wks.location}/CSLab2/Source/Application/**.h",
        "%{wks.location}/CSLab2/Source/Application/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/CSLab2/Source/Application",
		"%{wks.location}/CSLab2/ThirdParty",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}"
    }

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}
    

	links 
	{ 
		"Glad",
		"GLFW",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        symbols "off"

