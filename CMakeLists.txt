cmake_minimum_required(VERSION 3.6)
#------------------------------------------------------------------------------

set(Radium_DIR "~/cpp-workspace/Radium-Engine/Bundle-GNU/lib/cmake/Radium/")

# Force debug mode
#SET(CMAKE_BUILD_TYPE Debug ... FORCE)

# Policies and global parameters for CMake
if (POLICY CMP0077)
    # allow to define options cache variable before the option is declared
    # https://cmake.org/cmake/help/latest/policy/CMP0077.html
    cmake_policy(SET CMP0077 NEW)
endif ()
if (APPLE)
    # MACOSX_RPATH is enabled by default.
    # https://cmake.org/cmake/help/latest/policy/CMP0042.html
    cmake_policy(SET CMP0042 NEW)
endif (APPLE)
cmake_policy(SET CMP0012 NEW)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(HyperV VERSION 1.0.0)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/installed-${CMAKE_CXX_COMPILER_ID}" CACHE PATH
        "Install path prefix, prepended onto install directories." FORCE)
    message("Set install prefix to ${CMAKE_INSTALL_PREFIX}")
    set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT False)
endif ()

option(WITH_H3D_SUPPORT "Compile with H3D loader support" OFF)

add_subdirectory(src)

# Add documentation directory
add_subdirectory(doc)
