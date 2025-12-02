cmake_minimum_required(VERSION 3.28.3)

# Project name and version
project(CrestEngine)

# Set C++ standard to C++14 (matching your project)
#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug-VR;Release")

# Configure vcpkg integration
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Add preprocessor definitions (matching your .vcxproj)
add_compile_definitions(
    XR_TUTORIAL_USE_OPENGL
    _CRT_SECURE_NO_WARNINGS
)

# Find required packages via vcpkg
find_package(OpenXR CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/CrestEngine
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Glad
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLM
)

# Collect source files from CrestEngine folder
file(GLOB CREST_ENGINE_SOURCES
    "CrestEngine/*.cpp"
)

file(GLOB CREST_ENGINE_HEADERS
    "CrestEngine/*.h"
)

# Collect source files from Dependencies/Common
file(GLOB COMMON_SOURCES
    "Dependencies/Common/*.cpp"
    "Dependencies/Common/*.c"
)

file(GLOB COMMON_HEADERS
    "Dependencies/Common/*.h"
)

# Create the executable
add_executable(CrestEngine
    ${CREST_ENGINE_SOURCES}
    ${CREST_ENGINE_HEADERS}
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

# Link libraries
target_link_libraries(CrestEngine PRIVATE
    OpenXR::openxr_loader
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    opengl32
)

# Set the output directory for the executable
set_target_properties(CrestEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# For Windows, copy DLLs to output directory
if(WIN32)
    add_custom_command(TARGET CrestEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Build complete. Executable in: ${CMAKE_BINARY_DIR}/bin"
    )
endif()