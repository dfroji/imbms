cmake_minimum_required(VERSION 3.12)

include(FetchContent)

project(ImBMS
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(sfml
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.1
)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.90.7
)

FetchContent_Declare(imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG v2.6
)

option(SFML_BUILD_AUDIO "Build audio" OFF)
option(SFML_BUILD_NETWORK "Build network" OFF)
FetchContent_MakeAvailable(sfml)

FetchContent_MakeAvailable(imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)
FetchContent_MakeAvailable(imgui-sfml)

file(GLOB imbms_src CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(imbms
    ${imbms_src}
)

target_link_libraries(imbms
    PUBLIC ImGui-SFML::ImGui-SFML
)
