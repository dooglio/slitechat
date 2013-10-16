# -*- cmake -*-
if( STANDALONE )
	include( FindCARes )
else( STANDALONE )
	if (WINDOWS)
		set(CARES_LIBRARIES
			optimized ${ARCH_PREBUILT_DIRS_RELEASE}/areslib.lib
			debug     ${ARCH_PREBUILT_DIRS_DEBUG}/areslib.lib
			)
	else (WINDOWS)
		set(CARES_LIBRARIES cares)
	endif (WINDOWS)
	#
	set(CARES_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include/ares)
endif( STANDALONE )

