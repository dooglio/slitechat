cmake_minimum_required (VERSION 2.6)
project(LLChatLib)

#set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(LLC_SOURCE_DIR ${PROJECT_SOURCE_DIR})

add_subdirectory(llcommon)
add_subdirectory(llcharacter)
add_subdirectory(llinventory)
add_subdirectory(llmath)
add_subdirectory(llmessage)
add_subdirectory(llprimitive)
add_subdirectory(llvfs)
add_subdirectory(llxml)

set( WANT_UNICODE true )

include(00-Common)
include(APR)
include(Boost)
include(CARes)
include(CURL)
include(EXPAT)
include(OpenSSL)
include(XmlRpcEpi)
include(ZLIB)

include_directories(
	${APR_INCLUDE_DIR}
    ${APRUTIL_INCLUDE_DIR}
	${BOOST_INCLUDE_DIRS}
	${CARES_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
    ${EXPAT_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIRS}
	${XMLRPCEPI_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    )

include_directories(
	${LLC_SOURCE_DIR}/llcommon	
	${LLC_SOURCE_DIR}/llcharacter	
	${LLC_SOURCE_DIR}/llinventory
	${LLC_SOURCE_DIR}/llmath	
	${LLC_SOURCE_DIR}/llmessage	
	${LLC_SOURCE_DIR}/llprimitive	
	${LLC_SOURCE_DIR}/llvfs		
	${LLC_SOURCE_DIR}/llxml		
    )

set( llchatlib_SOURCE_FILES
	Agent.cpp
	ChatterBox.cpp
	LLChatLib.cpp
	lleventpoll.cpp
	llsrv.cpp
	lluserauth.cpp
	llviewernetwork.cpp
	llviewerregion.cpp
	llvoavatar.cpp
	llvoavatardefines.cpp
	llwearable.cpp
	llworld.cpp
	llxmlrpctransaction.cpp
	GoogleTranslate.cpp
	GridList.cpp
	SLUrlUtils.cpp
	ManagerImpl.cpp
	StringImpl.cpp
	)

set( llchatlib_HEADER_FILES
	Agent.h
	ChatterBox.h
	LLChatLib.h
	LLChatLibExports.h
	lleventpoll.h
	llsrv.h
	lluserauth.h
	llviewernetwork.h
	llviewerregion.h
	llvoavatar.h
	llvoavatardefines.h
	llwearable.h
	llworld.h
	llxmlrpctransaction.h
	GoogleTranslate.h
	GridList.h
	SLUrlUtils.h
	noise.h
	ManagerImpl.h
	StringImpl.h
	)

list( APPEND llchatlib_SOURCE_FILES ${llchatlib_HEADER_FILES} )
set_source_files_properties( ${llchatlib_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE )


link_directories( ${ARCH_PREBUILT_DIRS}
	${LLC_SOURCE_DIR}/llcharacter
	${LLC_SOURCE_DIR}/llcommon
	${LLC_SOURCE_DIR}/llinventory
	${LLC_SOURCE_DIR}/llmath
	${LLC_SOURCE_DIR}/llmessage
	${LLC_SOURCE_DIR}/llprimitive
	${LLC_SOURCE_DIR}/llvfs
	${LLC_SOURCE_DIR}/llxml )

if (LINUX)
  set(DL_LIBRARY dl)
  set(PTHREAD_LIBRARY pthread)
else (LINUX)
  set(DL_LIBRARY "")
  set(PTHREAD_LIBRARY "")
endif (LINUX)

if (WINDOWS)
  set(WINDOWS_LIBRARIES
	  wldap32.lib
      ws2_32
      mswsock
      psapi
      winmm
      netapi32
      )
else (WINDOWS)
  set(WINDOWS_LIBRARIES "")
endif (WINDOWS)
    
mark_as_advanced(DL_LIBRARY PTHREAD_LIBRARY WINDOWS_LIBRARIES)


add_definitions( -DNO_PRECOMPILED_HEADERS )

add_library( LLChatLib STATIC ${llchatlib_SOURCE_FILES} )

#add_dependencies( LLChatLib
#    llmessage
#    llinventory
#    llvfs
#    llxml
#    llmath
#    llcommon
#    )

if (WINDOWS)
	set_target_properties(LLChatLib
		PROPERTIES
		LINK_FLAGS "/debug"
		LINK_FLAGS_DEBUG "/NODEFAULTLIB:MSVCRT.lib"
		LINK_FLAGS_RELEASE ""
		)
endif (WINDOWS)
