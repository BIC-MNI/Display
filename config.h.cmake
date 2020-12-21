#ifndef CONFIG_H
#define CONFIG_H

/* the configured options and settings for Display */
#cmakedefine PROJECT_NAME "@PROJECT_NAME@"
#cmakedefine PROJECT_VERSION "@PROJECT_VERSION@"
#cmakedefine GIT_BRANCH "@GIT_BRANCH@"
#cmakedefine GIT_COMMIT "@GIT_COMMIT@"

#cmakedefine MINC2 @MINC2@

#define PACKAGE "@PACKAGE_NAME@"
#define PACKAGE_NAME "@PACKAGE_NAME@"
#define PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"
#define PACKAGE_VERSION "@PACKAGE_VERSION@"
#define PACKAGE_STRING "@PACKAGE_STRING@"
#define VERSION PACKAGE_VERSION


#cmakedefine USE_MINC2 1
#define HARD_CODED_DISPLAY_DIRECTORY1 "@HARD_CODED_DISPLAY_DIRECTORY1@"
#define HARD_CODED_DISPLAY_DIRECTORY2 "@HARD_CODED_DISPLAY_DIRECTORY2@"


/* define to disable compiled-in fallback menu */
//#cmakedefine DISPLAY_DISABLE_MENU_FALLBACK 1

/* define to enable sulcal labelling using SEAL */
//#cmakedefine DISPLAY_USE_SEAL 1

/* Use the Apple OpenGL framework. */
#cmakedefine HAVE_APPLE_OPENGL_FRAMEWORK 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H 1

/* Define to 1 if you have the `m' library (-lm). */
#cmakedefine HAVE_LIBM 1

/* Define to 1 if you have the `malloc' library (-lmalloc). */
#cmakedefine HAVE_LIBMALLOC 1

/* define if function mallinfo() is available */
#cmakedefine HAVE_MALLINFO 1

/* Define to 1 if you have the <malloc.h> header file. */
#cmakedefine HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H 1

/* Define if you have POSIX threads libraries and header files. */
#cmakedefine HAVE_PTHREAD 1

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H 1

/* Define to 1 if you have the <windows.h> header file. */
#cmakedefine HAVE_WINDOWS_H 1

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
#cmakedefine PTHREAD_CREATE_JOINABLE 1

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if the X Window System is missing or not being used. */
#cmakedefine X_DISPLAY_MISSING 1

/* Define to 1 if you have the <GL/freeglut.h> header file. */
#cmakedefine HAVE_FREEGLUT_H

/* Define to 1 if the GIFTI library is present and should be used. */
#cmakedefine GIFTI_FOUND 1

#endif // CONFIG_H
