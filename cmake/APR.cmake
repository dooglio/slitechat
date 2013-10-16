# -*- cmake -*-

if( STANDALONE )
	include( FindAPR )
else( STANDALONE )
	if( WINDOWS )
		set(APR_LIBRARIES 
			debug ${ARCH_PREBUILT_DIRS_DEBUG}/apr-1.lib
			optimized ${ARCH_PREBUILT_DIRS_RELEASE}/apr-1.lib
			)
		set(APRUTIL_LIBRARIES 
			debug ${ARCH_PREBUILT_DIRS_DEBUG}/aprutil-1.lib
			optimized ${ARCH_PREBUILT_DIRS_RELEASE}/aprutil-1.lib
			)
		# Doesn't need to link with iconv.dll
		set(APRICONV_LIBRARIES "")
	elseif( DARWIN )
		set(APR_LIBRARIES 
			debug ${ARCH_PREBUILT_DIRS_DEBUG}/libapr-1.a
			optimized ${ARCH_PREBUILT_DIRS_RELEASE}/libapr-1.a
			)
		set(APRUTIL_LIBRARIES 
			debug ${ARCH_PREBUILT_DIRS_DEBUG}/libaprutil-1.a
			optimized ${ARCH_PREBUILT_DIRS_RELEASE}/libaprutil-1.a
			)
		set(APRICONV_LIBRARIES iconv)
	else( WINDOWS )
		set(APR_LIBRARIES apr-1)
		set(APRUTIL_LIBRARIES aprutil-1)
		set(APRICONV_LIBRARIES iconv)
	endif( WINDOWS )

	set(APR_INCLUDE_DIR ${LIBS_PREBUILT_DIR}/include/${LL_ARCH_DIR}/apr-1 )
endif( STANDALONE )
