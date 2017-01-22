# - Try to find the CAIRO libraries
# Once done this will define
#
#  CAIRO_FOUND - system has pam
#  CAIRO_INCLUDE_DIR - the pam include directory
#  CAIRO_LIBRARIES - libpam library

SET (CAIRO_INCLUDE_DIR "/usr/include/cairo/")

if (CAIRO_INCLUDE_DIR AND CAIRO_LIBRARY)
	# Already in cache, be silent
	set(CAIRO_FIND_QUIETLY TRUE)
endif (CAIRO_INCLUDE_DIR AND CAIRO_LIBRARY)

find_path(CAIRO_INCLUDE_DIR NAMES cairo.h)
find_library(CAIRO_LIBRARY cairo)
find_library(DL_LIBRARY dl)

if (CAIRO_INCLUDE_DIR AND CAIRO_LIBRARY)
	set(CAIRO_FOUND TRUE)
	if (DL_LIBRARY)
		set(CAIRO_LIBRARIES ${CAIRO_LIBRARY} ${DL_LIBRARY})
	else (DL_LIBRARY)
		set(CAIRO_LIBRARIES ${CAIRO_LIBRARY})
	endif (DL_LIBRARY)

	if (EXISTS ${CAIRO_INCLUDE_DIR}/Imlib/imlib2.h)
		# darwin claims to be something special
		set(HAVE_CAIRO_CAIRO_APPL_H 1)
	endif (EXISTS ${CAIRO_INCLUDE_DIR}/Imlib/imlib2.h)
endif (CAIRO_INCLUDE_DIR AND CAIRO_LIBRARY)

if (CAIRO_FOUND)
	if (NOT CAIRO_FIND_QUIETLY)
		message(STATUS "Found Cairo: ${CAIRO_LIBRARIES}")
	endif (NOT CAIRO_FIND_QUIETLY)
else (CAIRO_FOUND)
	if (CAIRO_FIND_REQUIRED)
		message(FATAL_ERROR "Cairo was not found")
	endif(CAIRO_FIND_REQUIRED)
endif (CAIRO_FOUND)

mark_as_advanced(CAIRO_INCLUDE_DIR CAIRO_LIBRARY DL_LIBRARY CAIRO_MESSAGE_CONST)
