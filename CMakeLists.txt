cmake_minimum_required(VERSION 3.21)
project(AE4080 LANGUAGES CXX)

# EquilFlow is supplied as an x64 MSVC binary, with separate CRTs per config.
if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "The supplied EquilFlow libraries require MSVC x64.")
endif()
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Build configurations" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(EquilFlow STATIC IMPORTED)
set_target_properties(EquilFlow PROPERTIES
    IMPORTED_CONFIGURATIONS "Debug;Release"
    IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/EquilFlow/Debug/EquilFlowLib.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/EquilFlow/Release/EquilFlowLib.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/EquilFlow"
)

add_executable(AE4080 src/main.cpp src/std_atmosphere.cpp src/shocks.cpp)
add_executable(HW1 examples/HW1.cpp)
foreach(target IN ITEMS AE4080 HW1)
    target_compile_features(${target} PRIVATE cxx_std_20)
    target_compile_options(${target} PRIVATE /W4 /utf-8)
    target_link_libraries(${target} PRIVATE EquilFlow)
    # The course library's private PDB files were not distributed.
    target_link_options(${target} PRIVATE /IGNORE:4099)
    set_target_properties(${target} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endforeach()
