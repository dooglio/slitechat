cmake_minimum_required (VERSION 2.6)

set( PROJECT_NAME xgridchat_setup )
project(${PROJECT_NAME})

include(00-Common)
include(APR)
include(Boost)
include(CARes)
include(CURL)
include(EXPAT)
include(OpenSSL)
include(qt4)
include(XmlRpcEpi)
include(ZLIB)

set( LLCHATLIB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/LLChatLib )

include_directories(
	${APR_INCLUDE_DIR}
	${BOOST_INCLUDE_DIRS}
    ${EXPAT_INCLUDE_DIRS}
	${EXTRA_INCLUDE_PATHS}
    ${ZLIB_INCLUDE_DIRS}
	${LLCHATLIB_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	)

set( SOURCE_FILES
	main.cpp
	GridTab.cpp
	MainWindow.cpp
	Tester.cpp
	Utility.cpp
	)
	
set( UI_FILES
	)
	
set( UI_HEADER_FILES
	GridTab.h
	MainWindow.h
	)

set( HEADER_FILES
	${UI_HEADER_FILES}
	Tester.h
	Utility.h
	)
	
QT4_WRAP_UI( UI_SRCS
	${UI_FILES}
	)
	
QT4_WRAP_CPP( MOC_SRCS
	${UI_HEADER_FILES}
	)
	
QT4_ADD_RESOURCES( QRES
	)

set( CMAKE_FILES
	${CMAKE_MODULE_PATH}/00-Common.cmake
	${CMAKE_MODULE_PATH}/APR.cmake
	${CMAKE_MODULE_PATH}/Boost.cmake
	${CMAKE_MODULE_PATH}/CARes.cmake
	${CMAKE_MODULE_PATH}/CURL.cmake
	${CMAKE_MODULE_PATH}/EXPAT.cmake
	${CMAKE_MODULE_PATH}/FindAPR.cmake
	${CMAKE_MODULE_PATH}/FindCARes.cmake
	${CMAKE_MODULE_PATH}/FindXmlRpcEpi.cmake
	${CMAKE_MODULE_PATH}/OpenSSL.cmake
	${CMAKE_MODULE_PATH}/qt4.cmake
	${CMAKE_MODULE_PATH}/Toolchain_mingw32.cmake
	${CMAKE_MODULE_PATH}/XmlRpcEpi.cmake
	${CMAKE_MODULE_PATH}/ZLIB.cmake
	)

if (WINDOWS)
	set( WIN32_PROPERTIES TRUE )
	ENABLE_LANGUAGE(RC)
	SET(CMAKE_RC_COMPILE_OBJECT
		"<CMAKE_RC_COMPILER> /fo <OBJECT> <SOURCE>")
elseif (MINGW32)
	set( WIN32_PROPERTIES TRUE )
	set( EXTRA_LIBRARIES iconv mingw32 mingwex mingwthrd )
	set(CMAKE_RC_COMPILER_INIT windres)
	ENABLE_LANGUAGE(RC)
	SET(CMAKE_RC_COMPILE_OBJECT
		"<CMAKE_RC_COMPILER> --include-dir=${CMAKE_SOURCE_DIR}/SLiteChat/res/ -o <OBJECT> --output-format=coff <SOURCE>")
elseif (LINUX)
	set( EXTRA_LIB_PATHS ${ARCH_PREBUILT_DIRS} )
	set( EXTRA_LIBRARIES idn )
	add_definitions( "-g" )
#    LIST(APPEND CMAKE_EXE_LINKER_FLAGS -Wl,--as-needed)
endif (WINDOWS)

if( WIN32_PROPERTIES )
	set( EXTRA_LIBRARIES
		${EXTRA_LIBRARIES}
		wldap32
		ws2_32
		mswsock
		psapi
		winmm
		netapi32
		comctl32
		rpcrt4
		)
	#
	mark_as_advanced(DL_LIBRARY PTHREAD_LIBRARY EXTRA_LIBRARIES)
	#
	set( RESOURCE_FILES
		)
	source_group( "Resource Files" FILES ${RESOURCE_FILES} )
	#
	list( APPEND SOURCE_FILES ${RESOURCE_FILES} )
else (WIN32_PROPERTIES)
	set( RESOURCE_FILES
			)
endif (WIN32_PROPERTIES)


if( DARWIN )
	set( RESOURCE_FILES
		${CMAKE_SOURCE_DIR}/SLiteChat/res/slitechat.icns
		)
	set_source_files_properties( ${RESOURCE_FILES}
		PROPERTIES HEADER_FILE_ONLY TRUE
		)
	set_source_files_properties(
		${CMAKE_SOURCE_DIR}/SLiteChat/res/slitechat.icns
		PROPERTIES
		MACOSX_PACKAGE_LOCATION Resources/
		)
	list( APPEND SOURCE_FILES ${RESOURCE_FILES} )
	list( APPEND QT_LIBRARIES "-framework IOKit")
	set( EXTRA_LIBRARIES iconv )
endif( DARWIN )

if( LINUX )
	set( EXTRA_LIBRARIES Xext idn Xrender SM Xt Xft glib-2.0 gthread-2.0 )
endif( LINUX )

source_group( "Generated files" FILES ${UI_SRCS} ${MOC_SRCS} ${QRES} )
source_group( "Ui Files" FILES ${UI_FILES} )
source_group( "CMake Files" FILES ${CMAKE_FILES} )

list( APPEND SOURCE_FILES ${HEADER_FILES} ${CMAKE_FILES} ${UI_SRCS} ${MOC_SRCS} ${QRES} )
set_source_files_properties( ${HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE )
set_source_files_properties( ${CMAKE_FILES} PROPERTIES HEADER_FILE_ONLY TRUE )

# Make sure the linker can find the library once it is built.
link_directories(
	${ARCH_PREBUILT_DIRS}
	${EXTRA_LIB_PATHS}
	${CMAKE_SOURCE_DIR}/SLiteChat
	${CMAKE_SOURCE_DIR}/LLChatLib
	)

add_executable(
		${PROJECT_NAME}
		WIN32
		MACOSX_BUNDLE
		${SOURCE_FILES}
		)

if( DARWIN )
	set_target_properties(
			${PROJECT_NAME}
			PROPERTIES
			MACOSX_BUNDLE_INFO_STRING "XGridChat_Setup"
			MACOSX_BUNDLE_ICON_FILE "slitechat.icns"
			MACOSX_BUNDLE_GUI_IDENTIFIER "XGridChat_Setup"
			MACOSX_BUNDLE_LONG_VERSION_STRING "0.6.0"
			MACOSX_BUNDLE_BUNDLE_NAME "XGridChat_Setup"
			MACOSX_BUNDLE_SHORT_VERSION_STRING "0.6"
			MACOSX_BUNDLE_COPYRIGHT "(c) 2009 by R. Douglas Barbieri"
			LINK_SEARCH_END_STATIC true
			)
endif( DARWIN )


target_link_libraries(
	${PROJECT_NAME}
	LLChatLib
    llmessage
    llinventory
    llvfs
    llxml
    llcharacter
    llcommon
    llprimitive
    llmath
	png
	${APR_LIBRARIES}
	${APRUTIL_LIBRARIES}
	${BOOST_LIBRARIES}
	${CARES_LIBRARIES}
	${CURL_LIBRARIES}
	${EXPAT_LIBRARIES}
	${OPENSSL_LIBRARIES}
	${QT_LIBRARIES}
	${XMLRPCEPI_LIBRARIES}
	${ZLIB_LIBRARIES}
	${EXTRA_LIBRARIES}
	) 

if (WINDOWS)
	set_target_properties(
		${PROJECT_NAME}
		PROPERTIES
        LINK_FLAGS "/debug /SUBSYSTEM:WINDOWS"
        LINK_FLAGS_DEBUG "/NODEFAULTLIB:MSVCRTD.lib;MSVCPRTD.lib"
        LINK_FLAGS_RELEASE "/NODEFAULTLIB:MSVCRT.lib;MSVCPRT.lib"
		)
endif (WINDOWS)

if(DARWIN)
	install( TARGETS ${PROJECT_NAME}
			RUNTIME
			BUNDLE DESTINATION Applications )
else(DARWIN)
	install( TARGETS ${PROJECT_NAME}
			RUNTIME
			DESTINATION bin )
endif(DARWIN)

# vim: ts=4 sw=4 noexpandtab
