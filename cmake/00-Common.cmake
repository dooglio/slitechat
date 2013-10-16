# -*- cmake -*-

# Portable compilation flags.
#
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -DLL_DEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE
	"-DLL_RELEASE=1 -DLL_RELEASE_FOR_DOWNLOAD=1 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO 
	"-DLL_RELEASE=1 -DNDEBUG -DLL_RELEASE_WITH_DEBUG_INFO=1")

set(STANDALONE FALSE CACHE BOOL "Use system libraries instead of those in 3rdParty")

if( STANDALONE )
	set(VERBOSE_FIND FALSE CACHE BOOL "Show location of found libraries")
	mark_as_advanced( VERBOSE_FIND )
else( STANDALONE )
	set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/3rdParty CACHE PATH "Location of third party binary tree")
	#
	if( NOT EXISTS ${THIRDPARTY_DIR} )
		message(FATAL_ERROR
		  "${THIRDPARTY_DIR} not found! Make sure you set the THIRDPARTY_DIR path var or set the STANDALONE switch!"
		  )
	endif( NOT EXISTS ${THIRDPARTY_DIR} )
	#
	set(LIBS_PREBUILT_DIR ${THIRDPARTY_DIR} )
endif( STANDALONE )

# Don't bother with a MinSizeRel build.
#
if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE "Release")
endif( NOT CMAKE_BUILD_TYPE )
#
set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo;Release;Debug" CACHE STRING
	"Supported build types." FORCSTANDALONEE)

option( DEVELOPER_MODE "Turn on to include debugging info" FALSE )

# Determine which platform we are on and set flags accordingly
#
if (UNIX)
	#
	# Linux
	#
	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(LINUX ON BOOL FORCE)
		execute_process(COMMAND uname -m COMMAND sed s/i.86/i686/
			OUTPUT_VARIABLE ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
		set(LL_ARCH ${ARCH}_linux)
		set(LL_ARCH_DIR ${ARCH}-linux)
	endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

	# Darwin
	#
	if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(DARWIN ON BOOL FORCE)
		# set this dynamically from the build system now -
		# NOTE: wont have a distributable build unless you add this on the configure line with:
		# -DCMAKE_OSX_ARCHITECTURES:STRING='i386;ppc'
		set(CMAKE_OSX_ARCHITECTURES i386)
		set(CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.4u.sdk)

		if (CMAKE_OSX_ARCHITECTURES MATCHES "i386" AND CMAKE_OSX_ARCHITECTURES MATCHES "ppc")
			set(ARCH universal)
		else (CMAKE_OSX_ARCHITECTURES MATCHES "i386" AND CMAKE_OSX_ARCHITECTURES MATCHES "ppc")
			if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc")
				set(ARCH ppc)
			else (${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc")
				set(ARCH i386)
			endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc")
		endif (CMAKE_OSX_ARCHITECTURES MATCHES "i386" AND CMAKE_OSX_ARCHITECTURES MATCHES "ppc")
		set(LL_ARCH ${ARCH}_darwin)
		set(LL_ARCH_DIR universal-darwin)
	endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

else (UNIX)
	#
	# Just windows
	#
	if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		if (MSVC)
			set(WINDOWS ON BOOL FORCE)
			set(ARCH i686)
			set(LL_ARCH ${ARCH}_win32)
			set(LL_ARCH_DIR ${ARCH}-win32)
		else (MSVC)
			set(MINGW32 ON BOOL FORCE)
			set(ARCH mingw)
			set(LL_ARCH ${ARCH}_win32)
			set(LL_ARCH_DIR ${ARCH}-win32)
		endif (MSVC)
	endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
endif (UNIX)


if (WINDOWS)
	# Don't build DLLs.
	# set(BUILD_SHARED_LIBS OFF)

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /MTd"
		CACHE STRING "C++ compiler debug options" FORCE)
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO 
		"${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Od /Zi /MT"
		CACHE STRING "C++ compiler release-with-debug options" FORCE)
	set(CMAKE_CXX_FLAGS_RELEASE
		"${CMAKE_CXX_FLAGS_RELEASE} ${LL_CXX_FLAGS} /O2 /Zi /MT"
		CACHE STRING "C++ compiler release options" FORCE)

	add_definitions(
		/DLL_WINDOWS=1
		/GS
		/TP
		/W3
		/c
		/Zc:forScope
		/nologo
		/Oy-
		)

	if( WANT_UNICODE )
		add_definitions(
			/DUNICODE
			/D_UNICODE 
			)
	endif (WANT_UNICODE )

	if(MSVC80 OR MSVC90)
		set(CMAKE_CXX_FLAGS_RELEASE
			"${CMAKE_CXX_FLAGS_RELEASE} -D_SECURE_STL=0 -D_HAS_ITERATOR_DEBUGGING=0"
			CACHE STRING "C++ compiler release options" FORCE)

		add_definitions(
			/Zc:wchar_t-
			)
	endif (MSVC80 OR MSVC90)

	# Are we using the crummy Visual Studio KDU build workaround?
	if (NOT VS_DISABLE_FATAL_WARNINGS)
		add_definitions(/WX)
	endif (NOT VS_DISABLE_FATAL_WARNINGS)
endif (WINDOWS)

if (MINGW32)
	if( DEVELOPER_MODE )
		set( EXTRA_FLAGS "-g3" )
	else( DEVELOPER_MODE )
		set( EXTRA_FLAGS "-O3" )
	endif( DEVELOPER_MODE )

	add_definitions(
		-DLL_MINGW32=1
		-D_WIN32_WINNT=0x0500
		-D_WIN32_IE=0x0500
		-D_REENTRANT
		-fexceptions
		-fno-math-errno
		-fno-strict-aliasing
		-fsigned-char
		${EXTRA_FLAGS}
		)

	set(CMAKE_CXX_LINK_FLAGS "-Wl,--enable-runtime-pseudo-reloc")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_CXX_LINK_FLAGS}")

	if( WANT_UNICODE )
		add_definitions(
			-DUNICODE
			-D_UNICODE 
			)
	endif (WANT_UNICODE )
endif (MINGW32)

if (LINUX)
	# GCC 4.3 introduces a pile of obnoxious new warnings, which we
	# treat as errors due to -Werror.  Quiet the most offensive and
	# widespread of them.
	if (${CXX_VERSION} MATCHES "4.3")
		add_definitions(-Wno-deprecated -Wno-parentheses)
	endif (${CXX_VERSION} MATCHES "4.3")

	set(CMAKE_CXX_LINK_FLAGS "-Wl,-rpath -Wl,.")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_CXX_LINK_FLAGS}")

	# End of hacks.

	if( DEVELOPER_MODE )
		set( EXTRA_FLAGS "-g3" )
	else( DEVELOPER_MODE )
		set( EXTRA_FLAGS "-O3" )
	endif( DEVELOPER_MODE )

	add_definitions(
		-DLL_LINUX=1
		-D_REENTRANT
		-fexceptions
		-fno-math-errno
		-fno-strict-aliasing
		-fsigned-char
		${EXTRA_FLAGS}
		-pthread
		)
endif (LINUX)

if (DARWIN)
	set( CMAKE_FIND_LIBRARY_SUFFIXES .a )
	add_definitions(-DLL_DARWIN=1)
	set(CMAKE_CXX_LINK_FLAGS "-Wl,-headerpad_max_install_names,-search_paths_first")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_CXX_LINK_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mlong-branch")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mlong-branch")
	# NOTE: it's critical that the optimization flag is put in front.
	# NOTE: it's critical to have both CXX_FLAGS and C_FLAGS covered.
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O0 ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O0 ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
endif (DARWIN)

if (LINUX OR DARWIN)
	set(GCC_WARNINGS "-Wall -Wno-sign-compare -Wno-trigraphs")

	option( GCC_DISABLE_FATAL_WARNINGS "Turn this on and warnings will no longer be treated as errors." FALSE )
	if (NOT GCC_DISABLE_FATAL_WARNINGS)
		set(GCC_WARNINGS "${GCC_WARNINGS} -Werror -Wno-deprecated")
	endif (NOT GCC_DISABLE_FATAL_WARNINGS)

	set(GCC_CXX_WARNINGS "${GCC_WARNINGS} -Wno-reorder -Wno-deprecated")

	set(CMAKE_C_FLAGS "${GCC_WARNINGS} ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${GCC_CXX_WARNINGS} ${CMAKE_CXX_FLAGS}")
endif (LINUX OR DARWIN)


# Handle library search path
#
if (WINDOWS)
	if( STANDALONE )
		add_definitions( /DLL_STANDALONE=1 )
	else( STANDALONE )
		set(ARCH_PREBUILT_DIRS			${LIBS_PREBUILT_DIR}/lib/${LL_ARCH_DIR})
		set(ARCH_PREBUILT_DIRS_RELEASE	${ARCH_PREBUILT_DIRS}/Release)
		set(ARCH_PREBUILT_DIRS_DEBUG	${ARCH_PREBUILT_DIRS}/Debug)
	endif( STANDALONE )
	#
	if( MSVC71 )
		set(MSVERSION "vc71")
	elseif( MSVC80 )
		set(MSVERSION "vc80")
	elseif( MSVC90 )
		set(MSVERSION "vc90")
	endif (MSVC71)
elseif (MINGW32)
	if( STANDALONE )
		add_definitions( -DLL_STANDALONE=1 )
	else( STANDALONE )
		set(ARCH_PREBUILT_DIRS			${LIBS_PREBUILT_DIR}/lib/${LL_ARCH_DIR})
		set(ARCH_PREBUILT_DIRS_RELEASE	${ARCH_PREBUILT_DIRS})
		set(ARCH_PREBUILT_DIRS_DEBUG	${ARCH_PREBUILT_DIRS})
	endif( STANDALONE )
elseif (LINUX)
	if( STANDALONE )
		add_definitions( -DLL_STANDALONE=1 )
	else( STANDALONE )
		set(ARCH_PREBUILT_DIRS			${LIBS_PREBUILT_DIR}/lib/${LL_ARCH_DIR})
		set(ARCH_PREBUILT_DIRS_RELEASE	${ARCH_PREBUILT_DIRS})
		set(ARCH_PREBUILT_DIRS_DEBUG	${ARCH_PREBUILT_DIRS})
	endif( STANDALONE )
elseif (DARWIN)
	if( STANDALONE )
		add_definitions( -DLL_STANDALONE=1 )
	else( STANDALONE )
		set(ARCH_PREBUILT_DIRS			${LIBS_PREBUILT_DIR}/lib/${LL_ARCH_DIR})
		set(ARCH_PREBUILT_DIRS_RELEASE	${ARCH_PREBUILT_DIRS})
		set(ARCH_PREBUILT_DIRS_DEBUG	${ARCH_PREBUILT_DIRS})
	endif( STANDALONE )
endif (WINDOWS)

# vim: ts=4 sw=4 noexpandtab
