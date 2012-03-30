cmake_minimum_required(VERSION 2.6)

# Project name
project (Display C)

# Include modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

#----------------------------------------------------------------------
# Set the version number using git
if( EXISTS "${CMAKE_SOURCE_DIR}/.git" )
	execute_process(COMMAND git describe --tags HEAD
					OUTPUT_VARIABLE PROJECT_VERSION
					OUTPUT_STRIP_TRAILING_WHITESPACE)
endif( EXISTS "${CMAKE_SOURCE_DIR}/.git")

if( "${PROJECT_VERSION}" STREQUAL "" )
    set (PROJECT_VERSION 1.6.0)
endif( "${PROJECT_VERSION}" STREQUAL "" )

message(STATUS "Project version: ${PROJECT_VERSION}")
#----------------------------------------------------------------------


# Strange variables needed by Display
set(HARD_CODED_DISPLAY_DIRECTORY1 "/usr/local/mni/lib" CACHE PATH "HARD_CODED_DISPLAY_DIRECTORY1")
set(HARD_CODED_DISPLAY_DIRECTORY2 "/usr/local/lib" CACHE PATH "HARD_CODED_DISPLAY_DIRECTORY1")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
   "${PROJECT_SOURCE_DIR}/config.h.cmake"
   "${PROJECT_BINARY_DIR}/config.h"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")
include_directories(Graphics/OpenGL_graphics/Include Graphics/Include Graphics/GLUT_windows/Include Include)

# Quick and dirty fix to force static libraries
option(DISPLAY_BUILD_STATIC "Build the binary with statically linked libraries" OFF)
if (DISPLAY_BUILD_STATIC)
  if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  else(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  endif(WIN32)
endif (DISPLAY_BUILD_STATIC)

# NetCDF
set(NetCDF_ROOT "/usr/local")
find_package(NetCDF REQUIRED)
include_directories( ${NetCDF_INCLUDE_DIR} )

# HDF5
if (DISPLAY_BUILD_STATIC)
  option(HDF5_USE_STATIC_LIBRARIES "Use HDF5 static libraries" ON)
endif (DISPLAY_BUILD_STATIC)
find_package(HDF5 REQUIRED)
include_directories( ${HDF5_INCLUDE_DIRS} )
add_definitions(${HDF5_DEFINITIONS})
# This is needed by the static library HDF5
#SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")

# MINC
option(USE_MINC2 "Use MINC2 libraries" ON)
add_definitions(-DMINC2)
find_package(MINC REQUIRED)
include_directories( ${MINC_INCLUDE_DIR} )
# add_library(minc2 STATIC IMPORTED)
# set_property(TARGET minc2 PROPERTY IMPORTED_LOCATION ${MINC_minc2_LIBRARY})
# add_library(volume_io2 STATIC IMPORTED)
# set_property(TARGET volume_io2 PROPERTY IMPORTED_LOCATION ${MINC_volume_io2_LIBRARY})

# ZLIB
find_package(ZLIB REQUIRED)
include_directories( ${ZLIB_INCLUDE_DIR} )

# BICpl
find_package(BICPL REQUIRED)
include_directories( ${BICPL_INCLUDE_DIR} )
# add_library(bicpl STATIC IMPORTED)
# set_property(TARGET bicpl PROPERTY IMPORTED_LOCATION ${BICPL_LIBRARY})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIR} )

# GLUT
find_package(GLUT REQUIRED)
include_directories( ${GLUT_INCLUDE_DIR} )


# Look for C source files. Dirty but easy way.
file (GLOB_RECURSE C_SRC "./*.c")
file (GLOB_RECURSE C_REMOVE "./*.include.c" "./*supports_gl.c" "./*CMakeCCompilerId.c")
list (REMOVE_ITEM C_SRC ${C_REMOVE})

#message(STATUS "${C_SRC}")

#----------------------------------------------------------------------
# RPATH handling
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

#SET(CMAKE_INSTALL_RPATH "${MINC_INCLUDE_DIR}/../lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
# LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
# IF("${isSystemDir}" STREQUAL "-1")
#    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# ENDIF("${isSystemDir}" STREQUAL "-1")

message(STATUS "RPATH: ${CMAKE_INSTALL_RPATH}")
#----------------------------------------------------------------------

# Compile each source file to a module
add_executable (Display ${C_SRC})

# Link all modules to create the executable
if (DISPLAY_BUILD_STATIC)
  set (LINK_LIBRARIES   
      ${MINC_LIBRARIES}
      ${NetCDF_C_LIBRARY}
      ${HDF5_hdf5_LIBRARY_RELEASE} ${HDF5_hdf5_hl_LIBRARY_RELEASE}
      ${ZLIB_LIBRARIES}
      ${BICPL_LIBRARY}
      ${GLUT_glut_LIBRARY} 
      ${OPENGL_gl_LIBRARY}
      -pthread)
else (DISPLAY_BUILD_STATIC)
  set (LINK_LIBRARIES   
      ${MINC_LIBRARIES}
      ${NetCDF_C_LIBRARY}
      ${HDF5_C_LIBRARIES}
      ${ZLIB_LIBRARIES}
      ${BICPL_LIBRARY}
      ${GLUT_glut_LIBRARY} 
      ${OPENGL_gl_LIBRARY})
endif (DISPLAY_BUILD_STATIC)

target_link_libraries(Display ${LINK_LIBRARIES})
  
# Generate an install target
install(TARGETS Display RUNTIME DESTINATION bin)


