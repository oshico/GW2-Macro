# =============================================================================
# PROJECT CONFIGURATION
# Purpose: Define project name, minimum CMake version, and C++ standard
# =============================================================================
cmake_minimum_required(VERSION 3.20)
project(Macro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================================================
# SOURCE FILE DEFINITIONS
# Purpose: Collect all source files using GLOB (simple for this project size)
# Note: Headers are NOT needed in SOURCES - they're found via include directories
# =============================================================================
file(GLOB SOURCES
        "src/Resource.rc"
        "src/core/*.cpp"
        "src/macro/*.cpp"
        "src/gui/*.cpp"
        "src/persistence/*.cpp"
        "src/compliance/*.cpp"
        "src/utils/*.cpp"
        "src/imgui/*.cpp"
)

# =============================================================================
# TARGET DEFINITION
# Purpose: Create the shared library (DLL) target
# =============================================================================
add_library(Macro SHARED ${SOURCES})

# =============================================================================
# INCLUDE DIRECTORIES
# Purpose: Tell compiler where to find headers for this target
# These are PRIVATE because only this library needs them
# =============================================================================
target_include_directories(Macro PRIVATE
        "src"
        "src/core"
        "src/macro"
        "src/gui"
        "src/persistence"
        "src/compliance"
        "src/utils"
        "src/imgui"
        "src/mumble"
        "src/nexus"
        "src/nlohmann"
)

# =============================================================================
# COMPILE DEFINITIONS
# Purpose: Define preprocessor macros for Windows API version
# PRIVATE means these only apply when compiling this target
# =============================================================================
target_compile_definitions(Macro PRIVATE
        _WIN32_WINNT=0x0600
        WINVER=0x0600
)

# =============================================================================
# COMPILE OPTIONS (PER-BUILD-TYPE)
# Purpose: Set compiler flags that differ between Debug and Release
#
# Debug flags:
#   -g        : Generate debug symbols (uncomment below if needed)
#
# Release flags:
#   -O3       : Maximum optimization level
#   -flto     : Link-time optimization for better performance
#   -g        : (Uncomment if you need debug symbols in release)
# =============================================================================
target_compile_options(Macro PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>

        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-flto>
        # $<$<CONFIG:Release>:-g>
)

# =============================================================================
# LINK OPTIONS (PER-BUILD-TYPE)
# Purpose: Set linker flags for static linking and optimizations
#
# Default linkers:
#   -static-libgcc -static-libstdc++ : Statically link GCC/C++ runtime
#   -Wl,--export-all-symbols         : Export all symbols (needed for DLL addon)
#
# Release linkers:
#   -s                                : Strip symbols in release (smaller DLL)
#   -flto                             : Link-time optimization (matches compile flag)
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
# Purpose: Set output names and directories for the built DLL
# PREFIX "" removes the "lib" prefix (so output is "Macro.dll" not "libMacro.dll")
# =============================================================================
set_target_properties(Macro PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
