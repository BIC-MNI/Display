cmake_minimum_required(VERSION 2.6)
project (Display)

# The version number
if( EXISTS "${CMAKE_SOURCE_DIR}/.git" )
	execute_process(COMMAND git describe --tags HEAD
					OUTPUT_VARIABLE PROJECT_VERSION
					OUTPUT_STRIP_TRAILING_WHITESPACE)
else( EXISTS "${CMAKE_SOURCE_DIR}/.git")
	set (PROJECT_VERSION 1.6.0)
endif( EXISTS "${CMAKE_SOURCE_DIR}/.git")


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


set(BICPL_ROOT "/usr/local" CACHE PATH "BICPL library base")
include_directories("${BICPL_ROOT}/include")
link_directories("${BICPL_ROOT}/lib")

option(USE_MINC2 "Use minc 2 library" ON)
set(MINC_ROOT "/usr/local" CACHE PATH "MINC library base")
link_directories("${MINC_ROOT}/lib")
include_directories("${MINC_ROOT}/include")

find_package(GLUT)
find_package(OpenGL)

if(USE_MINC2)
  add_definitions(-DMINC2)
endif(USE_MINC2)

set(HARD_CODED_DISPLAY_DIRECTORY1 "/usr/local/mni/lib" CACHE PATH "HARD_CODED_DISPLAY_DIRECTORY1")
set(HARD_CODED_DISPLAY_DIRECTORY2 "/usr/local/lib" CACHE PATH "HARD_CODED_DISPLAY_DIRECTORY1")

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

if(USE_MINC2)
  add_library(volume_io2 STATIC IMPORTED)
  set_property(TARGET volume_io2 PROPERTY IMPORTED_LOCATION /usr/local/mni/lib/libvolume_io2.a)

  add_library(minc2 STATIC IMPORTED)
  set_property(TARGET minc2 PROPERTY IMPORTED_LOCATION /usr/local/mni/lib/libminc2.a)

  target_link_libraries(Display volume_io2 minc2 netcdf hdf5 z bicpl ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
else(USE_MINC2)
  target_link_libraries(Display volume_io minc netcdf bicpl ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
endif(USE_MINC2)

install(TARGETS Display RUNTIME DESTINATION bin)


