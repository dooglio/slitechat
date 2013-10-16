# -*- cmake -*-
if( STANDALONE )
	include( FindCURL )
else( STANDALONE )
	if (WINDOWS)
		set(CURL_LIBRARIES
			optimized ${ARCH_PREBUILT_DIRS_RELEASE}/libcurl.lib
			debug     ${ARCH_PREBUILT_DIRS_DEBUG}/libcurld.lib
		)
		add_definitions( /DCURL_STATICLIB )
	else (WINDOWS)
		set(CURL_LIBRARIES libcurl.a)
	endif (WINDOWS)

	set(CURL_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include)
endif( STANDALONE )
