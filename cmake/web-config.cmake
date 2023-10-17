# WEB CMake configuration file:
# This file is meant to be placed in a cmake subfolder of WEB-devel-2.x.y-VC

cmake_minimum_required(VERSION 3.0)

include(FeatureSummary)
set_package_properties(WEB PROPERTIES
    URL "https://g10.app/status/#abstract_data_i"
    DESCRIPTION "Useful abstractions for the internet"
)

# Copied from `configure_package_config_file`
macro(set_and_check _var _file)
    set(${_var} "${_file}")
    if(NOT EXISTS "${_file}")
        message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
    endif()
endmacro()

# Copied from `configure_package_config_file`
macro(check_required_components _NAME)
    foreach(comp ${${_NAME}_FIND_COMPONENTS})
        if(NOT ${_NAME}_${comp}_FOUND)
            if(${_NAME}_FIND_REQUIRED_${comp})
                set(${_NAME}_FOUND FALSE)
            endif()
        endif()
    endforeach()
endmacro()

set(WEB_FOUND TRUE)

# For compatibility with autotools WEB-config.cmake, provide WEB_* variables.

set_and_check(WEB_PREFIX       "${CMAKE_CURRENT_LIST_DIR}/..")
set_and_check(WEB_EXEC_PREFIX  "${CMAKE_CURRENT_LIST_DIR}/..")
set_and_check(WEB_INCLUDE_DIR  "${WEB_PREFIX}/include")
set(WEB_INCLUDE_DIRS           "${WEB_INCLUDE_DIR}")
set_and_check(WEB_BINDIR       "${CMAKE_CURRENT_LIST_DIR}/../build/Debug/")
set_and_check(WEB_LIBDIR       "${CMAKE_CURRENT_LIST_DIR}/../build/Debug/")

set(WEB_LIBRARIES WEB::WEB)

# All targets are created, even when some might not be requested though COMPONENTS.
# This is done for compatibility with CMake generated WEB-target.cmake files.

set(_WEB_library     "${WEB_LIBDIR}/web.lib")
set(_WEB_dll_library "${WEB_BINDIR}/web.dll")
if(EXISTS "${_WEB_library}" AND EXISTS "${_WEB_dll_library}")
    if(NOT TARGET WEB::WEB)
        add_library(WEB::WEB SHARED IMPORTED)
        set_target_properties(WEB::WEB
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${WEB_INCLUDE_DIRS}"
                IMPORTED_IMPLIB "${_WEB_library}"
                IMPORTED_LOCATION "${_WEB_dll_library}"
                COMPATIBLE_INTERFACE_BOOL "WEB_SHARED"
                INTERFACE_WEB_SHARED "ON"
        )
    endif()
    set(WEB_WEB_FOUND TRUE)
else()
    set(WEB_WEB_FOUND FALSE)
endif()
unset(_WEB_library)
unset(_WEB_dll_library)

check_required_components(WEB)
