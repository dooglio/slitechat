# -*- cmake -*-

# - Find Apache Portable Runtime
# Find the APR includes and libraries
# This module defines
#  APR_INCLUDE_DIR and APRUTIL_INCLUDE_DIR, where to find apr.h, etc.
#  APR_LIBRARIES and APRUTIL_LIBRARIES, the libraries needed to use APR.
#  APR_FOUND and APRUTIL_FOUND, If false, do not try to use APR.
# also defined, but not for general use are
#  APR_LIBRARY and APRUTIL_LIBRARY, where to find the APR library.

# APR first.

FIND_PATH(APR_INCLUDE_DIR apr.h
/usr/local/include/apr-1
/usr/local/include/apr-1.0
/usr/include/apr-1
/usr/include/apr-1.0
/opt/local/include/apr-1
/opt/local/include/apr-1.0
)

SET(APR_NAMES ${APR_NAMES} apr-1)
FIND_LIBRARY(APR_LIBRARY
  NAMES ${APR_NAMES}
  PATHS /usr/lib /usr/local/lib /opt/local/lib
  )

IF (APR_LIBRARY AND APR_INCLUDE_DIR)
    SET(APR_LIBRARIES ${APR_LIBRARY})
    SET(APR_FOUND "YES")
ELSE (APR_LIBRARY AND APR_INCLUDE_DIR)
  SET(APR_FOUND "NO")
ENDIF (APR_LIBRARY AND APR_INCLUDE_DIR)


IF (APR_FOUND)
	if( VERBOSE_FIND )
		MESSAGE(STATUS "Found APR: ${APR_LIBRARIES}")
	endif( VERBOSE_FIND )
ELSE (APR_FOUND)
	MESSAGE(FATAL_ERROR "Could not find APR library")
ENDIF (APR_FOUND)

MARK_AS_ADVANCED(
  APR_LIBRARY
  APR_INCLUDE_DIR
  )

# Next, APRUTIL.

FIND_PATH(APRUTIL_INCLUDE_DIR apu.h
/usr/local/include/apr-1
/usr/local/include/apr-1.0
/usr/include/apr-1
/usr/include/apr-1.0
/opt/local/include/apr-1
/opt/local/include/apr-1.0
)

SET(APRUTIL_NAMES ${APRUTIL_NAMES} aprutil-1)
FIND_LIBRARY(APRUTIL_LIBRARY
  NAMES ${APRUTIL_NAMES}
  PATHS /usr/lib /usr/local/lib /opt/local/lib
  )

IF (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)
    SET(APRUTIL_LIBRARIES ${APRUTIL_LIBRARY})
    SET(APRUTIL_FOUND "YES")
ELSE (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)
  SET(APRUTIL_FOUND "NO")
ENDIF (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)


IF (APRUTIL_FOUND)
   if( VERBOSE_FIND )
      MESSAGE(STATUS "Found APRUTIL: ${APRUTIL_LIBRARIES}")
   endif (VERBOSE_FIND)
ELSE (APRUTIL_FOUND)
   IF (APRUTIL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find APRUTIL library")
   ENDIF (APRUTIL_FIND_REQUIRED)
ENDIF (APRUTIL_FOUND)

MARK_AS_ADVANCED(
  APRUTIL_LIBRARY
  APRUTIL_INCLUDE_DIR
  )
