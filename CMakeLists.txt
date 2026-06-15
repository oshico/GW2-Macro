# =============================================================================
# PROJECT CONFIGURATION
# Defines project name, minimum CMake version, and C++ standard
# =============================================================================
cmake_minimum_required(VERSION 3.20)
project(Macro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================================================
# SOURCE FILE DEFINITIONS
# Collects all source files using GLOB (simple for this project size)
# =============================================================================
file(GLOB SOURCES
        "MacroManager.rc"
        "src/*.cpp"
        "src/imgui/*.cpp"
)

# =============================================================================
# TARGET DEFINITION
# Creates the shared library (DLL) target
# =============================================================================
add_library(Macro SHARED ${SOURCES})

# =============================================================================
# INCLUDE DIRECTORIES
# Tells compiler where to find headers for this target
# PRIVATE sets them for this target only
# =============================================================================
target_include_directories(Macro PRIVATE
        "src"
        "src/nexus"
        "src/imgui"
        "src/mumble"
        "src/nlohmann"
)

# =============================================================================
# COMPILE DEFINITIONS
# Defines preprocessor macros for Windows API version
# PRIVATE sets them for this target only
# =============================================================================
target_compile_definitions(Macro PRIVATE
        _WIN32_WINNT=0x0600
        WINVER=0x0600
)

# =============================================================================
# COMPILE OPTIONS (PER-BUILD-TYPE)
# Sets compiler flags that differ between Debug and Release
#
# Debug flags:
#   -g        : Generate debug symbols
#
# Release flags:
#   -O3       : Maximum optimization level
#   -flto     : Link-time optimization for better performance
# =============================================================================
target_compile_options(Macro PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>

        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-flto>
)

# =============================================================================
# LINK OPTIONS (PER-BUILD-TYPE)
# Set linker flags for static linking and optimizations
#
# Default linkers:
#   -static-libgcc -static-libstdc++ : Statically link GCC/C++ runtime
#   -Wl,--export-all-symbols         : Export all symbols
#
# Release linkers:
#   -s                                : Strip symbols in release
#   -flto                             : Link-time optimization
# =============================================================================
target_link_libraries(Macro PRIVATE comdlg32)

target_link_options(Macro PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
        -Wl,--export-all-symbols

        $<$<CONFIG:Release>:-s>
        $<$<CONFIG:Release>:-flto>
)

# =============================================================================
# TARGET PROPERTIES
# Sets output names and directories for the built DLL
# PREFIX "" removes the "lib" prefix (output is "Macro.dll" instead "libMacro.dll")
# =============================================================================
set_target_properties(Macro PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)