cmake_minimum_required (VERSION 2.6)

set( PROJECT_NAME slitechat )
project(${PROJECT_NAME})

# The application version number.
#
set( slitechat_VERSION_MAJOR 1 )
set( slitechat_VERSION_MINOR 6 )
set( slitechat_VERSION_PATCH 3 )

configure_file( 
	"${PROJECT_SOURCE_DIR}/Config.h.in"
	"${PROJECT_BINARY_DIR}/Config.h"
	)	

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

if( WINDOWS )
	set(SHOW_CONSOLE_WINDOW FALSE CACHE BOOL "Show console window under MS Windows")
	if( SHOW_CONSOLE_WINDOW )
		add_definitions( "-DSHOW_CONSOLE_WINDOW" )
	endif( SHOW_CONSOLE_WINDOW )
endif( WINDOWS )

include_directories(
	${APR_INCLUDE_DIR}
	${BOOST_INCLUDE_DIRS}
    ${EXPAT_INCLUDE_DIRS}
	${EXTRA_INCLUDE_PATHS}
    ${ZLIB_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/LLChatLib
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	)

set( SOURCE_FILES
	ChatWindow.cpp
	Common.cpp
	ExportWindow.cpp
	LoginWindow.cpp
	MainWindow.cpp
	MessageDialog.cpp
	Preferences.cpp
	SearchWindow.cpp
	Utility.cpp
	main.cpp
	)
	
set( UI_FILES
	ChatWindow.ui
	ExportWindow.ui
	LoginWindow.ui
	MainWindow.ui
	MessageDialog.ui
	Preferences.ui
	SearchWindow.ui
	)
	
set( UI_HEADER_FILES
	ChatWindow.h
	ExportWindow.h
	LoginWindow.h
	MainWindow.h
	MessageDialog.h
	Preferences.h
	SearchWindow.h
	)

set( HEADER_FILES
	${UI_HEADER_FILES}
	Config.h
	Common.h
	Utility.h
	)
	
QT4_WRAP_UI( UI_SRCS
	${UI_FILES}
	)
	
QT4_WRAP_CPP( MOC_SRCS
	${UI_HEADER_FILES}
	)
	
QT4_ADD_RESOURCES( QRES
	resources.qrc
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
		resources.qrc
		res/resource.rc
		res/slitechat_128.png
		res/SLITECHAT32.ico
		res/webdev-alert-128x128.png
		res/webdev-cancel-128x128.png
		res/webdev-home-128x128.png
		res/webdev-site-map-128x128.png
		res/webdev-user-128x128.png
		)
	source_group( "Resource Files" FILES ${RESOURCE_FILES} )
	#
	list( APPEND HEADER_FILES res/resource.h )
	list( APPEND SOURCE_FILES ${RESOURCE_FILES} )
else (WIN32_PROPERTIES)
	set( RESOURCE_FILES res/SLITECHAT32.xpm )
endif (WIN32_PROPERTIES)


# Install common files
file(GLOB appset_files
		"${CMAKE_SOURCE_DIR}/doc/app_settings/*.xml"
		"${CMAKE_SOURCE_DIR}/doc/app_settings/*.pem"
		"${CMAKE_SOURCE_DIR}/doc/app_settings/*.msg"
		)
#
file(GLOB character_files
		"${CMAKE_SOURCE_DIR}/doc/character/*.xml"
		)

if( DARWIN )
	set( RESOURCE_FILES
		${appset_files}
		${character_files}
		${CMAKE_SOURCE_DIR}/SLiteChat/res/slitechat.icns
		)
	set_source_files_properties( ${RESOURCE_FILES}
		PROPERTIES HEADER_FILE_ONLY TRUE
		)
	set_source_files_properties(
		${appset_files}
		PROPERTIES
		MACOSX_PACKAGE_LOCATION Resources/app_settings/
		)
	set_source_files_properties(
		${character_files}
		PROPERTIES
		MACOSX_PACKAGE_LOCATION Resources/character/
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
			MACOSX_BUNDLE_INFO_STRING "SLiteChat"
			MACOSX_BUNDLE_ICON_FILE "slitechat.icns"
			MACOSX_BUNDLE_GUI_IDENTIFIER "SLiteChat"
			MACOSX_BUNDLE_LONG_VERSION_STRING "0.6.0"
			MACOSX_BUNDLE_BUNDLE_NAME "SLiteChat"
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


##########################################################################3
# Install section
##########################################################################3

# Set up CPACK definitions
#
include(InstallRequiredSystemLibraries)
set(INSTALL_PREFIX ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SLiteChat")
set(CPACK_PACKAGE_VENDOR "Made to Order Software Corporation")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/doc/gpl.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${slitechat_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${slitechat_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${slitechat_VERSION_PATCH})
set(CPACK_SOURCE_PACKAGE_FILE_NAME "slitechat-src-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}.tar.gz")

if( NOT DEVELOPER_MODE )
	set(CPACK_STRIP_FILES TRUE )
endif( NOT DEVELOPER_MODE )

if(WINDOWS)
	set( BUILD_WIN32_PACKAGES TRUE )
endif(WINDOWS)
if(MINGW32)
	set( BUILD_WIN32_PACKAGES TRUE )
endif(MINGW32)

if(BUILD_WIN32_PACKAGES)
	set(CPACK_GENERATOR "ZIP;NSIS")

	INSTALL(TARGETS slitechat RUNTIME DESTINATION bin)
	if(WINDOWS)
		INSTALL(FILES
			${ARCH_PREBUILT_DIRS_RELEASE}/xmlrpcepi.dll
			DESTINATION bin 
			)
	else(WINDOWS)
		INSTALL(FILES
			${ARCH_PREBUILT_DIRS_RELEASE}/mingwm10.dll
			DESTINATION bin 
			)
	endif(WINDOWS)
	
	INSTALL(FILES
		${QT_LIBRARY_DIR}/../bin/QtCore4.dll
		${QT_LIBRARY_DIR}/../bin/QtGui4.dll
		${QT_LIBRARY_DIR}/../bin/QtNetwork4.dll
		DESTINATION bin
		)

	INSTALL(FILES ${appset_files}
		DESTINATION bin/app_settings
		)
	INSTALL(FILES ${character_files}
		DESTINATION bin/character
		)
	INSTALL(FILES
		${CMAKE_SOURCE_DIR}/doc/gpl.txt 
		"${CMAKE_SOURCE_DIR}/SLiteChat/res/slitechat_128.png"
		DESTINATION share/doc
		)

	# There is a bug in NSI that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backlasshes.
	#
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "Made to Order Software Corporation\\\\SLiteChat")
	set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/SLiteChat/res\\\\slitechat_128.png")
	set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\\\\bin/slitechat.exe")
	set(CPACK_NSIS_DISPLAY_NAME "SLiteChat for Windows")
	set(CPACK_NSIS_HELP_LINK "http://www.slitechat.org/")
	set(CPACK_NSIS_URL_INFO_ABOUT "http://www.slitechat.org/")
	set(CPACK_NSIS_CONTACT "contact@m2osw.com")
	set(CPACK_NSIS_MODIFY_PATH ON)
	set(CPACK_PACKAGE_EXECUTABLES "slitechat" "SLiteChat - Text-based IM client for Second Life")
endif(BUILD_WIN32_PACKAGES)

if(LINUX)
	set(CPACK_GENERATOR "TGZ;DEB")
	INSTALL(FILES ${appset_files} DESTINATION share/slitechat/app_settings)
	INSTALL(FILES ${character_files} DESTINATION share/slitechat/character)
	INSTALL(FILES ${CMAKE_SOURCE_DIR}/doc/gpl.txt 
			DESTINATION share/doc/slitechat)
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/SLiteChat/res/slitechat_128.png"
			DESTINATION share/doc/slitechat)
	INSTALL(TARGETS slitechat RUNTIME DESTINATION bin)

	set(CPACK_PACKAGE_INSTALL_DIRECTORY "slitechat")
	set(CPACK_STRIP_FILES "bin/slitechat")
	set(CPACK_PACKAGE_CONTACT "contact@m2osw.com")
	set(CPACK_PACKAGE_FILE_NAME
			"slitechat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-Linux-${CMAKE_SYSTEM_PROCESSOR}")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.9), libstdc++6 (>= 4.3.3), libidn11 (>= 1.10-3), libx11-6 (>= 2:1.1.99.2), libxext6 (>= 2:1.0.99.1), libpng12-0 (>= 1.2.0)")
endif(LINUX)

if(DARWIN)
	set(CPACK_GENERATOR "PackageMaker")
	set(CPACK_PACKAGE_EXECUTABLES "slitechat" "SLiteChat - Text-based IM client for Second Life")
	set(CPACK_PACKAGING_INSTALL_PREFIX "/")
	INSTALL( TARGETS slitechat
			 RUNTIME
			 BUNDLE DESTINATION Applications
			)
endif(DARWIN)

include(CPack)

# vim: ts=4 sw=4 noexpandtab
