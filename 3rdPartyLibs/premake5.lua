workspace "ARBITRONIC_3RDPARTYLIBS"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }
--All the 3rd party dependencies
include "GLFW"
include "glad_OpenGL"
include "spdlog"
include "imgui"
include "stb"