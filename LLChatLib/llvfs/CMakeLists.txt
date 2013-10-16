# -*- cmake -*-

project(llvfs)

set( WANT_UNICODE true )

include(00-Common)
include(APR)
include(Boost)
include(EXPAT)
include(ZLIB)

include_directories(
    ${APR_INCLUDE_DIR}
    ${APRUTIL_INCLUDE_DIR}
	${BOOST_INCLUDE_DIRS}
	${LLC_SOURCE_DIR}/llcommon
    ${LLC_SOURCE_DIR}/llvfs
    )

set(llvfs_SOURCE_FILES
    lldir.cpp
    lllfsthread.cpp
    llvfile.cpp
    llvfs.cpp
    llvfsthread.cpp
    )

set(llvfs_HEADER_FILES
    lldir.h
    lllfsthread.h
    llvfile.h
    llvfs.h
    llvfsthread.h
    )

if (DARWIN)
  LIST(APPEND llvfs_SOURCE_FILES lldir_mac.cpp)
  LIST(APPEND llvfs_HEADER_FILES lldir_mac.h)
  set( DARWIN_APPLICATION_NAME "SLiteChat"
      CACHE PATH "Name for Mac OS/X local config folder: e.g. $HOME/Library/Application Support/<DARWIN_APPLICATION_NAME>/user_settings" )
  add_definitions( "-DDARWIN_APPLICATION_NAME=\"${DARWIN_APPLICATION_NAME}\"" )
endif (DARWIN)

if (LINUX)
  LIST(APPEND llvfs_SOURCE_FILES lldir_linux.cpp)
  LIST(APPEND llvfs_HEADER_FILES lldir_linux.h)

  if (VIEWER AND INSTALL)
    set_source_files_properties(lldir_linux.cpp
                                PROPERTIES COMPILE_FLAGS
                                "-DAPP_RO_DATA_DIR=\\\"${APP_SHARE_DIR}\\\""
                                )
  endif (VIEWER AND INSTALL)
endif (LINUX)

if (WINDOWS)
  LIST(APPEND llvfs_SOURCE_FILES lldir_win32.cpp)
  LIST(APPEND llvfs_HEADER_FILES lldir_win32.h)
endif (WINDOWS)

if (MINGW32)
  LIST(APPEND llvfs_SOURCE_FILES lldir_win32.cpp)
  LIST(APPEND llvfs_HEADER_FILES lldir_win32.h)
endif (MINGW32)

set_source_files_properties(${llvfs_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llvfs_SOURCE_FILES ${llvfs_HEADER_FILES})

add_library (llvfs STATIC ${llvfs_SOURCE_FILES})

if (DARWIN)
  include(CMakeFindFrameworks)
  find_library(CARBON_LIBRARY Carbon)
  target_link_libraries(llvfs ${CARBON_LIBRARY})
endif (DARWIN)

# vim: ts=4 sw=4
