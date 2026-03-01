
project "_ArbitronicCompute2D"
    kind "ConsoleApp"
    language "C++"

    objdir ("binaries/objs_int")

    files{
        "src/**.h",
        "src/**.cpp",
        "src/**.c",
        "src/**.hpp"
    }

    includedirs{
       "$(SolutionDir)/Engine/src",
       "$(SolutionDir)/Engine/src/Core",
       "$(SolutionDir)/3rdPartyLibs",
       "$(SolutionDir)/3rdPartyLibs/GLFW/include",
       "$(SolutionDir)/3rdPartyLibs/glad_OpenGL/include",
       "$(SolutionDir)/3rdPartyLibs/spdlog/include",
       "$(SolutionDir)/3rdPartyLibs/imgui/src/main/",
       "$(SolutionDir)/3rdPartyLibs/imgui/src/backend",
       "$(SolutionDir)/3rdPartyLibs/Glm",
       "$(SolutionDir)/3rdPartyLibs/fmt/include",
       "$(SolutionDir)/3rdPartyLibs/stb"
    }
    
    libdirs{
        "$(SolutionDir)3rdPartyLibs/fmt/build/Debug",
        "$(SolutionDir)3rdPartyLibs/glad_OpenGL/binaries/GLAD",
        "$(SolutionDir)3rdPartyLibs/GLFW/binaries/GLFW",
        "$(SolutionDir)3rdPartyLibs/imgui/binaries/imgui",
        "$(SolutionDir)3rdPartyLibs/spdlog/binaries/spdlog",
        "$(SolutionDir)3rdPartyLibs/stb/binaries/STB"
    }

    pchheader "ARBpch.h"
    pchsource "src/Core/ARBpch.cpp"

    links
    {
        "GLFW.lib", --linking the project with name "GLFW",
        "GLAD.lib",
        "opengl32.lib",
        "spdlog.lib",
        "imgui.lib",
        "fmtd.lib",
        "STB.lib"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        buildoptions
        {
            "/utf-8"
        }
        defines 
		{
			"_CRT_SECURE_NO_WARNINGS" --pre-defined macro to remove all warnings
		}

    filter "configurations:Debug"
        targetdir ("binaries/Main/Debug")
        defines "ARB_DEBUG"
        symbols "On"

    filter "configurations:Release"
        targetdir ("binaries/Main/Release")
        defines "ARB_RELEASE"
        optimize "On"