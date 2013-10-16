# -*- cmake -*-
if( STANDALONE )
	set(OpenSSL_FIND_REQUIRED ON)
	include( FindOpenSSL )
	if( DARWIN )
		# UGLY HACK!
		# This is a problem under mac--it finds the one in /usr/lib which does not work
		find_library( CRYPTO_LIB name crypto PATHS /opt/local/lib )
		list( APPEND OPENSSL_LIBRARIES ${CRYPTO_LIB} )
	endif( DARWIN )
else( STANDALONE )
	if (WINDOWS)
		set(OPENSSL_LIBRARIES
			${ARCH_PREBUILT_DIRS}/ssleay32.lib ${ARCH_PREBUILT_DIRS}/libeay32.lib
			)
	else (WINDOWS)
		set(OPENSSL_LIBRARIES ssl)
		set(CRYPTO_LIBRARIES crypto)
	endif (WINDOWS)

	set(OPENSSL_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include)
endif( STANDALONE )
