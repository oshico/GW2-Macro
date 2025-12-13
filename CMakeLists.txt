cmake_minimum_required(VERSION 3.20)
project(Macro)

# Simple static build for Nexus
set(CMAKE_CXX_STANDARD 17)

# Force static linking
set(CMAKE_CXX_FLAGS "-static -static-libgcc -static-libstdc++ -D_WIN32_WINNT=0x0600 -DWINVER=0x0600")
set(CMAKE_SHARED_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,--export-all-symbols")

# Sources
file(GLOB SOURCES "src/*.cpp" "src/imgui/*.cpp")
file(GLOB HEADERS "src/*.h" "src/imgui/*.h" "src/mumble/*.h" "src/nexus/*.h" "src/nlohmann/*.hpp")

add_library(Macro SHARED ${SOURCES} src/Resource.rc)

# Include directories
target_include_directories(Macro PRIVATE 
    src
    src/imgui
    src/mumble
    src/nexus
    src/nlohmann
)

# No lib prefix
set_target_properties(Macro PROPERTIES PREFIX "")

# Output to bin directory
set_target_properties(Macro PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)