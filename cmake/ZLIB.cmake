# -*- cmake -*-

if( STANDALONE )
	include( FindZLIB )
else( STANDALONE )
	if (WINDOWS)
		set(ZLIB_LIBRARIES
			optimized ${ARCH_PREBUILT_DIRS_RELEASE}/zlib.lib
			debug     ${ARCH_PREBUILT_DIRS_DEBUG}/zlibd.lib
			)
	else (WINDOWS)
		set(ZLIB_LIBRARIES z.a)
	endif (WINDOWS)

	set(ZLIB_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include/zlib)
endif( STANDALONE )
