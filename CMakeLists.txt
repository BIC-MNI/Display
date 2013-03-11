# CMakeFiles.txt for the Display
#
# Project name
project (Display C)

cmake_minimum_required(VERSION 2.6)


SET(DISPLAY_PACKAGE_VERSION_MAJOR 1)
SET(DISPLAY_PACKAGE_VERSION_MINOR 6)
SET(DISPLAY_PACKAGE_VERSION_PATCH 0)


IF(NOT MINC_TOOLKIT_BUILD)
  
  SET(CPACK_PACKAGE_VERSION_MAJOR ${DISPLAY_PACKAGE_VERSION_MAJOR})
  SET(CPACK_PACKAGE_VERSION_MINOR ${DISPLAY_PACKAGE_VERSION_MINOR})
  SET(CPACK_PACKAGE_VERSION_PATCH ${DISPLAY_PACKAGE_VERSION_PATCH})
  INCLUDE(CPack)
  
  # Include modules
  SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

ENDIF(NOT MINC_TOOLKIT_BUILD)
  
SET(PACKAGE "Display")
SET(PACKAGE_BUGREPORT "a.janke@gmail.com")

SET(PACKAGE_NAME "Display")
SET(PACKAGE_VERSION "${DISPLAY_PACKAGE_VERSION_MAJOR}.${DISPLAY_PACKAGE_VERSION_MINOR}.${DISPLAY_PACKAGE_VERSION_PATCH}")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")


#----------------------------------------------------------------------

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# aliases
SET(VERSION "${PACKAGE_VERSION}")
SET(PROJECT_VERSION "${PACKAGE_VERSION}")

# check for prereqs
INCLUDE(CheckIncludeFiles)

CHECK_INCLUDE_FILES(float.h     HAVE_FLOAT_H)
CHECK_INCLUDE_FILES(dlfcn.h     HAVE_FLFCN_H)
CHECK_INCLUDE_FILES(sys/stat.h  HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(values.h    HAVE_VALUES_H)
CHECK_INCLUDE_FILES(unistd.h    HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(memory.h    HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(malloc.h    HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(stdlib.h    HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(fcntl.h     HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(vfork.h     HAVE_VFORK_H)
CHECK_INCLUDE_FILES(inttypes.h  HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(string.h    HAVE_STRING_H)
CHECK_INCLUDE_FILES(strings.h   HAVE_STRINGS_H)

# CHECK_INCLUDE_FILES(bicpl/deform.h HAVE_DEFORM_H)

IF(CMAKE_USE_PTHREADS_INIT)
  SET(HAVE_PTHREAD ON)
ELSE(CMAKE_USE_PTHREADS_INIT)
  SET(HAVE_PTHREAD OFF)
ENDIF(CMAKE_USE_PTHREADS_INIT)

IF(APPLE)
  SET(HAVE_APPLE_OPENGL_FRAMEWORK ON)
  SET(X_DISPLAY_MISSING ON)
ENDIF(APPLE)

IF(WINDOWS)
  CHECK_INCLUDE_FILES(windows.h   HAVE_WINDOWS_H)
ELSE(WINDOWS)
  SET(HAVE_WINDOWS_H OFF)
ENDIF(WINDOWS)

# Strange variables needed by Display
set(HARD_CODED_DISPLAY_DIRECTORY1 "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
set(HARD_CODED_DISPLAY_DIRECTORY2 "/usr/local/lib${LIB_SUFFIX}")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
   "${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake"
   "${CMAKE_CURRENT_BINARY_DIR}/config.h"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories( Include )


IF(NOT MINC_TOOLKIT_BUILD)
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

  # HDF5
  if (DISPLAY_BUILD_STATIC)
    option(HDF5_USE_STATIC_LIBRARIES "Use HDF5 static libraries" ON)
  endif (DISPLAY_BUILD_STATIC)
  find_package(HDF5 REQUIRED)
  # This is needed by the static library HDF5
  #SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")

  # MINC
  find_package(LIBMINC REQUIRED)
  # add_library(minc2 STATIC IMPORTED)
  # set_property(TARGET minc2 PROPERTY IMPORTED_LOCATION ${MINC_minc2_LIBRARY})
  # add_library(volume_io2 STATIC IMPORTED)
  # set_property(TARGET volume_io2 PROPERTY IMPORTED_LOCATION ${MINC_volume_io2_LIBRARY})

  # LIBM
  #find_package(LibM REQUIRED)
  #include_directories( ${LibM_INCLUDE_DIR} )

  # ZLIB
  find_package(ZLIB REQUIRED)

  # BICpl
  find_package(BICPL REQUIRED)

  # BICGL
  find_package(bicgl REQUIRED)

  # add_library(bicpl STATIC IMPORTED)
  # set_property(TARGET bicpl PROPERTY IMPORTED_LOCATION ${BICPL_LIBRARY})

  # OpenGL
  find_package(OpenGL REQUIRED)

  # GLUT
#   FIND_PACKAGE( Threads )
  #FIND_PACKAGE( GL REQUIRED )
  #FIND_PACKAGE( GLU REQUIRED )
  find_package( GLUT REQUIRED )

ELSE(NOT MINC_TOOLKIT_BUILD)
  INCLUDE( ${LIBMINC_USE_FILE} )
ENDIF(NOT MINC_TOOLKIT_BUILD)

include_directories( ${BICGL_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${BICGL_LIBRARY_DIRS} )

include_directories( ${NetCDF_INCLUDE_DIR} )
include_directories( ${HDF5_INCLUDE_DIRS} )
add_definitions( ${HDF5_DEFINITIONS} )
include_directories( ${MINC_INCLUDE_DIR} )
include_directories( ${ZLIB_INCLUDE_DIR} )
include_directories( ${BICPL_INCLUDE_DIR} )
include_directories( ${BICGL_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${GLUT_INCLUDE_DIR} )
#include_directories( ${GLU_INCLUDE_DIR} )


# Look for C source files. Dirty but easy way.
# file (GLOB_RECURSE C_SRC "./*.c")
# file (GLOB_RECURSE C_REMOVE "./*.include.c" "./*supports_gl.c" "./*CMakeCCompilerId.c")
# list (REMOVE_ITEM C_SRC ${C_REMOVE})

SET(C_SRC 
           main/main.c 
           main/display.c 
           main/event_loop.c 
           main/graphics.c 
           main/three_d.c 
           main/transforms.c 
           atlas/atlas.c 
           input_files/input_files.c 
           input_files/volume_file.c 
           callbacks/atlas.c 
           callbacks/call_globals.c 
           callbacks/colour_coding.c 
           callbacks/file.c 
           callbacks/line_ops.c 
           callbacks/object_ops.c 
           callbacks/marker_ops.c 
           callbacks/polygon_ops.c 
           callbacks/quit.c 
           callbacks/regions.c 
           callbacks/render_ops.c 
           callbacks/segmenting.c 
           callbacks/surf_segmenting.c 
           callbacks/surface_curves.c 
           callbacks/surface_extract.c 
           callbacks/view_ops.c 
           callbacks/volume_ops.c 
           callbacks/volume_transform_ops.c 
           current_obj/current_obj.c 
           edit_surface/connected.c 
           edit_surface/edit.c 
           edit_surface/segment.c 
           images/images.c 
           markers/markers.c 
           surface_extraction/boundary_extraction.c 
           surface_extraction/data_structs.c 
           surface_extraction/init_surface.c 
           surface_extraction/extract.c 
           surface_extraction/surface.c 
           surface_extraction/surface_events.c 
           voxel_scan/scan_objects.c 
           events/change_markers.c 
           events/clip_plane.c 
           events/film_loop.c 
           events/magnify.c 
           events/mouse.c 
           events/mouse_trans.c 
           events/pick_object.c 
           events/pick_view.c 
           events/rotate_slice.c 
           events/spaceball.c 
           events/virt_sb.c 
           events/window_man.c 
           events/utilities.c 
           immediate_mode/draw_immed.c 
           intersect/ray_polygons.c 
           intersect/plane_polygons.c 
           cursor/cursor.c 
           cursor/cursor_icon.c 
           menu/build_menu.c 
           menu/cursor_pos.c 
           menu/menu.c 
           menu/input_menu.c 
           menu/selected.c 
           menu/text.c 
           cursor_contours/contours.c 
           segmenting/cut_neighbours.c 
           segmenting/painting.c 
           segmenting/segmenting.c 
           segmenting/segment_polygons.c 
           slice_window/colour_bar.c 
           slice_window/colour_coding.c 
           slice_window/crop.c 
           slice_window/draw_slice.c 
           slice_window/histogram.c 
           slice_window/pick_angle.c 
           slice_window/quadmesh.c 
           slice_window/slice.c 
           slice_window/slice_3d.c 
           slice_window/slice_events.c 
           slice_window/undo.c 
           slice_window/view.c 
           surface_curves/closest_line.c 
           surface_curves/events.c 
           surface_curves/edge_distance.c 
           tubes/convert_lines.c 
           structures/action_table.c 
           structures/fit_view.c 
           structures/lights.c 
           structures/render.c 
           structures/view.c 
           structures/window.c
           stack/stack.c
)

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
# LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" isSystemDir)
# IF("${isSystemDir}" STREQUAL "-1")
#    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
# ENDIF("${isSystemDir}" STREQUAL "-1")

#message(STATUS "RPATH: ${CMAKE_INSTALL_RPATH}")
#----------------------------------------------------------------------

# Compile each source file to a module
add_executable (Display  ${C_SRC}  )

# Link all modules to create the executable
IF(NOT MINC_TOOLKIT_BUILD)
  if (DISPLAY_BUILD_STATIC)
    set (LINK_LIBRARIES   
        ${LIBMINC_LIBRARIES}
        ${NetCDF_C_LIBRARY}
        ${HDF5_hdf5_hl_LIBRARY_RELEASE} ${HDF5_hdf5_LIBRARY_RELEASE} 
        ${ZLIB_LIBRARIES}
        ${BICPL_LIBRARY}
        ${GLUT_glut_LIBRARY} 
        ${OPENGL_gl_LIBRARY}
        -lm -pthread)
  else (DISPLAY_BUILD_STATIC)
    set (LINK_LIBRARIES   
        ${MINC_LIBRARIES}
        ${NetCDF_C_LIBRARY}
        ${HDF5_C_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${BICPL_LIBRARY}
        ${GLUT_glut_LIBRARY} 
        ${OPENGL_gl_LIBRARY} )
  endif (DISPLAY_BUILD_STATIC)
ELSE(NOT MINC_TOOLKIT_BUILD)
    set (LINK_LIBRARIES   
      ${GLUT_LIBRARIES} ${OPENGL_gl_LIBRARY}
      ${BICPL_LIBRARIES} ${VOLUME_IO_LIBRARIES} ${LIBMINC_LIBRARIES}
        -lm -pthread)
ENDIF(NOT MINC_TOOLKIT_BUILD)

target_link_libraries(Display ${BICGL_LIBRARIES} ${LINK_LIBRARIES})
  
# Generate an install target
install(TARGETS Display RUNTIME DESTINATION bin)
INSTALL(FILES Display.globals DESTINATION lib${LIB_SUFFIX})


