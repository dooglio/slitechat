cmake_minimum_required (VERSION 2.6)

add_subdirectory( setup )

set( PROJECT_NAME xgridchat )
project( ${PROJECT_NAME} )

include(00-Common)
include(01-Configure)

include(APR)
include(Boost)
include(CARes)
include(CURL)
include(EXPAT)
include(OpenSSL)
include(XmlRpcEpi)
include(ZLIB)

# The version number for XGridChat.
#
set( xgridchat_VERSION_MAJOR 0 )
set( xgridchat_VERSION_MINOR 3 )
set( xgridchat_VERSION_PATCH 0 )

configure_file( 
	"${PROJECT_SOURCE_DIR}/version.h.in"
	"${PROJECT_BINARY_DIR}/version.h"
	)	

ConfigureSystemIncludes()

configure_file(
	"${PROJECT_SOURCE_DIR}/config.h.cmake"
	"${PROJECT_BINARY_DIR}/config.h"
	)

include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_BINARY_DIR}
	${BOOST_INCLUDE_DIRS}
	${LLChatLib_SOURCE_DIR}
    )

if( WINDOWS )
	add_definitions( -D_CRT_SECURE_NO_DEPRECATE )
endif( WINDOWS )

set( SOURCE_FILES
		Robot.cpp
		main.cpp
	)

set( HEADER_FILES
		Robot.h
	)

list( APPEND SOURCE_FILES ${HEADER_FILES} )
set_source_files_properties( ${HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE )


if( LINUX )
	set( EXTRA_LIBRARIES
		idn
		crypto
		pthread
		dl
		rt
	   )
elseif( DARWIN )
	set( EXTRA_LIBRARIES
			iconv
			"-framework IOKit"
			)
elseif( MINGW32 )
	set( EXTRA_LIBRARIES
			iconv
			mingw32
			mingwex
			mingwthrd
	   )
elseif( WINDOWS )
	set( EXTRA_LIBRARIES
		wldap32
		ws2_32
		mswsock
		psapi
		winmm
		netapi32
		comctl32
		rpcrt4
		)
endif( LINUX )

link_directories( 
	${LLChatLib_BINARY_DIR}
	${ARCH_PREBUILT_DIRS}
	)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

target_link_libraries( ${PROJECT_NAME}
		LLChatLib
		llmessage
		llinventory
		llvfs
		llxml
		llcharacter
		llcommon
		llprimitive
		llmath
		${APR_LIBRARIES}
		${APRUTIL_LIBRARIES}
		${BOOST_LIBRARIES}
		${CARES_LIBRARIES}
		${CURL_LIBRARIES}
		${EXPAT_LIBRARIES}
		${OPENSSL_LIBRARIES}
		${XMLRPCEPI_LIBRARIES}
		${ZLIB_LIBRARIES}
		${EXTRA_LIBRARIES}
	)

install( TARGETS xgridchat RUNTIME DESTINATION bin )

# vim: ts=4 sw=4 noexpandtab
