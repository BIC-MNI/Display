# FindMINC.cmake module


FIND_PATH(MINC_INCLUDE_DIR minc2.h /usr/include /usr/local/include /usr/local/bic/include)
FIND_LIBRARY(MINC_volume_io2_LIBRARY NAMES volume_io2 HINTS /usr/lib /usr/local/lib /usr/local/bic/lib)
FIND_LIBRARY(MINC_minc2_LIBRARY NAMES minc2 HINTS /usr/lib /usr/local/lib /usr/local/bic/lib)


IF (MINC_INCLUDE_DIR AND MINC_minc2_LIBRARY AND MINC_volume_io2_LIBRARY)
   set(MINC_LIBRARIES 
     ${MINC_volume_io2_LIBRARY}
     ${MINC_minc2_LIBRARY} 
     )
   SET(MINC_FOUND TRUE)
   
ENDIF (MINC_INCLUDE_DIR AND MINC_minc2_LIBRARY AND MINC_volume_io2_LIBRARY)


IF (MINC_FOUND)
   IF (NOT Minc_FIND_QUIETLY)
      MESSAGE(STATUS "Found MINC: ${MINC_LIBRARIES}")
   ENDIF (NOT Minc_FIND_QUIETLY)
ELSE (MINC_FOUND)
   IF (Minc_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Cound not find MINC")
   ENDIF (Minc_FIND_REQUIRED)
ENDIF (MINC_FOUND)


mark_as_advanced(
  MINC_minc2_LIBRARY
  MINC_volume_io2_LIBRARY
)
