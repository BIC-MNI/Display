cmake_minimum_required(VERSION 2.6)
project (Display)

# The version number
set (Display_VERSION_MAJOR 1)
set (Display_VERSION_MINOR 6)

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file (
#  "${PROJECT_SOURCE_DIR}/config.h.in"
#  "${PROJECT_BINARY_DIR}/config.h"
#  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")
 

include_directories(Graphics/OpenGL_graphics/Include Graphics/Include Graphics/GLUT_windows/Include Include)


set(BICPL_ROOT "/usr/local" CACHE PATH "BICPL library base")
include_directories("${BICPL_ROOT}/include")
link_directories("${BICPL_ROOT}/lib")
link_libraries(bicpl)

option(USE_MINC2 "Use minc 2 library" ON)
set(MINC_ROOT "/usr/local" CACHE PATH "MINC library base")
link_directories("${MINC_ROOT}/lib")
include_directories("${MINC_ROOT}/include")
if(USE_MINC2)
  add_definitions(-DMINC2)
  link_libraries(volume_io2 minc2 netcdf hdf5 z)
else(USE_MINC2)
  link_libraries(volume_io minc netcdf)
endif(USE_MINC2)

find_package(GLUT)
find_package(OpenGL)
link_libraries (${GLUT_LIBRARY} ${OPENGL_LIBRARY})


set(DISPLAY_DIR "/usr/local" CACHE PATH "HARD_CODED_DISPLAY_DIRECTORY")

add_definitions(-DHARD_CODED_DISPLAY_DIRECTORY1="${DISPLAY_DIR}")
#add_definitions(-Wall -Wextra)

#set (SRC_DIR atlas callbacks current_obj cursor	cursor_contours	edit_surface events)
#list (APPEND ${SRC_DIR}	Graphics Hardware images immediate_mode	input_files)
#list (APPEND ${SRC_DIR} intersect main markers menu	segmenting slice_window)
#list (APPEND ${SRC_DIR}	structures surface_curves surface_extraction tubes voxel_scan)
#add_subdirectory (atlas main)

file (GLOB_RECURSE C_SRC "./*.c")
file (GLOB_RECURSE C_REMOVE "./*.include.c" "./*supports_gl.c" "./*CMakeCCompilerId.c")
list (REMOVE_ITEM C_SRC ${C_REMOVE})

add_executable (Display ${C_SRC})


