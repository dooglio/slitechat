################################################################################
# Copyright (c) 2009 Made to Order Software Corporation
#
# Permission to redistribute with copyright notice intact.
#
# Author: R. Douglas Barbieri
#
# This file inspects the current system so you can produce a proper "config.h" file.
# The project can then properly compile and link against the current local libraries.
#
# For example:
# include( 01-Configure )
# ConfigureSystemIncludes()
# configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )
#
################################################################################


################################################################################
# ConfigureSystemIncludes()
#
# This function checks for common include files then configures the system,
# Call configure_file() to generate the proper include file.
#
function( ConfigureSystemIncludes )
	#
	include( CheckIncludeFiles )
	include( CheckFunctionExists )
	include( CheckSymbolExists )
	include( CheckLibraryExists )
	include( CheckTypeSize )
	include( MoCheckCSourceCompiles )

	if( NOT WINDOWS )
		add_definitions( -D_GNU_SOURCE )
		set( CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE )
	endif( NOT WINDOWS )

	#
	# This macro checks if the symbol exists in the library and if it
	# does, it appends library to the list.
	SET(REQUIRED_LIBS "")
	MACRO(CHECK_LIBRARY_EXISTS_CONCAT LIBRARY SYMBOL VARIABLE)
	  CHECK_LIBRARY_EXISTS("${LIBRARY};${REQUIRED_LIBS}" ${SYMBOL} ""
		${VARIABLE})
	  IF(${VARIABLE})
		SET(REQUIRED_LIBS ${REQUIRED_LIBS} ${LIBRARY})
	  ENDIF(${VARIABLE})
	ENDMACRO(CHECK_LIBRARY_EXISTS_CONCAT)

	# Check for all needed libraries
	CHECK_LIBRARY_EXISTS_CONCAT("dl"     dlopen       HAVE_LIBDL)
	CHECK_LIBRARY_EXISTS_CONCAT("ucb"    gethostname  HAVE_LIBUCB)
	CHECK_LIBRARY_EXISTS_CONCAT("socket" connect      HAVE_LIBSOCKET)
	CHECK_LIBRARY_EXISTS("c" gethostbyname "" NOT_NEED_LIBNSL)
	#
	IF(NOT NOT_NEED_LIBNSL)
	  CHECK_LIBRARY_EXISTS_CONCAT("nsl"    gethostbyname  HAVE_LIBNSL)
	ENDIF(NOT NOT_NEED_LIBNSL)
	#
	CHECK_LIBRARY_EXISTS_CONCAT("ws2_32"	WSAGetLastError	HAVE_LIBWS2_32)
	CHECK_LIBRARY_EXISTS_CONCAT("winmm" 	puts			HAVE_LIBWINMM)
	CHECK_LIBRARY_EXISTS_CONCAT("m"			cos 			HAVE_LIBWINMM)
	CHECK_LIBRARY_EXISTS_CONCAT("history"	append_history	HAVE_LIBHISTORY)
	CHECK_LIBRARY_EXISTS_CONCAT("readline"	readline		HAVE_LIBREADLINE)
	CHECK_LIBRARY_EXISTS_CONCAT("pthread"	pthread_create	HAVE_LIBPTHREAD)
	#
	#OPTION(CMAKE_USE_OPENSSL "Use OpenSSL code. Experimental" OFF)
	#
	MARK_AS_ADVANCED(CMAKE_USE_OPENSSL)
	IF(CMAKE_USE_OPENSSL)
	  CHECK_LIBRARY_EXISTS_CONCAT("crypto" CRYPTO_lock  HAVE_LIBCRYPTO)
	  CHECK_LIBRARY_EXISTS_CONCAT("ssl"    SSL_connect  HAVE_LIBSSL)
	ENDIF(CMAKE_USE_OPENSSL)

	# Check for symbol dlopen (same as HAVE_LIBDL)
	CHECK_LIBRARY_EXISTS("${REQUIRED_LIBS}" dlopen "" HAVE_DLOPEN)
	CHECK_LIBRARY_EXISTS("${REQUIRED_LIBS}" finite "" HAVE_FINITE)
	CHECK_LIBRARY_EXISTS("${REQUIRED_LIBS}" fprintf "" HAVE_FPRINTF)
	CHECK_LIBRARY_EXISTS("${REQUIRED_LIBS}" ftime "" HAVE_FTIME)

	# For other tests to use the same libraries
	SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${REQUIRED_LIBS})

	# If we have features.h, then do the _BSD_SOURCE magic
	CHECK_INCLUDE_FILES("features.h"       HAVE_FEATURES_H)

	# Check if header file exists and add it to the list.
	MACRO(CHECK_INCLUDE_FILES_CONCAT FILE VARIABLE)
	  CHECK_INCLUDE_FILES("${REQUIRED_INCLUDES};${FILE}" ${VARIABLE})
	  IF(${VARIABLE})
		SET(REQUIRED_INCLUDES ${REQUIRED_INCLUDES} ${FILE})
	  ENDIF(${VARIABLE})
	ENDMACRO(CHECK_INCLUDE_FILES_CONCAT)

	# Check for header files
	CHECK_INCLUDE_FILES_CONCAT("arpa/inet.h"      HAVE_ARPA_INET_H)
	CHECK_INCLUDE_FILES_CONCAT("arpa/nameser.h"  HAVE_ARPA_NAMESER_H)
	CHECK_INCLUDE_FILES_CONCAT("stdio.h"          HAVE_STDIO_H)
	CHECK_INCLUDE_FILES_CONCAT("stddef.h"         HAVE_STDDEF_H)
	CHECK_INCLUDE_FILES_CONCAT("inttypes.h"       HAVE_INTTYPES_H)
	CHECK_INCLUDE_FILES_CONCAT("alloca.h"         HAVE_ALLOCA_H)
	CHECK_INCLUDE_FILES_CONCAT("dlfcn.h"          HAVE_DLFCN_H)
	CHECK_INCLUDE_FILES_CONCAT("fcntl.h"          HAVE_FCNTL_H)
	CHECK_INCLUDE_FILES_CONCAT("malloc.h"         HAVE_MALLOC_H)
	CHECK_INCLUDE_FILES_CONCAT("memory.h"         HAVE_MEMORY_H)
	CHECK_INCLUDE_FILES_CONCAT("netdb.h"          HAVE_NETDB_H)
	CHECK_INCLUDE_FILES_CONCAT("assert.h"         HAVE_ASSERT_H)
	CHECK_INCLUDE_FILES_CONCAT("limits.h"         HAVE_LIMITS_H)

	IF(CMAKE_USE_OPENSSL)
	  CHECK_INCLUDE_FILES_CONCAT("openssl/x509.h"   HAVE_OPENSSL_X509_H)
	  CHECK_INCLUDE_FILES_CONCAT("openssl/engine.h" HAVE_OPENSSL_ENGINE_H)
	  CHECK_INCLUDE_FILES_CONCAT("openssl/rsa.h"    HAVE_OPENSSL_RSA_H)
	  CHECK_INCLUDE_FILES_CONCAT("openssl/crypto.h" HAVE_OPENSSL_CRYPTO_H)
	  CHECK_INCLUDE_FILES_CONCAT("openssl/pem.h"    HAVE_OPENSSL_PEM_H)
	  CHECK_INCLUDE_FILES_CONCAT("openssl/ssl.h"    HAVE_OPENSSL_SSL_H)
	  CHECK_INCLUDE_FILES_CONCAT("openssl/err.h"    HAVE_OPENSSL_ERR_H)
	  CHECK_INCLUDE_FILES_CONCAT("openssl/rand.h"   HAVE_OPENSSL_RAND_H)
	ENDIF(CMAKE_USE_OPENSSL)

	CHECK_INCLUDE_FILES_CONCAT("zlib.h"           HAVE_ZLIB_H)
	CHECK_INCLUDE_FILES_CONCAT("netinet/in.h"     HAVE_NETINET_IN_H)
	CHECK_INCLUDE_FILES_CONCAT("net/if.h"         HAVE_NET_IF_H)
	CHECK_INCLUDE_FILES_CONCAT("netinet/if_ether.h" HAVE_NETINET_IF_ETHER_H)
	CHECK_INCLUDE_FILES_CONCAT("netinet/tcp.h"	HAVE_NETINET_TCP_H)
	CHECK_INCLUDE_FILES_CONCAT("utime.h"         HAVE_UTIME_H)
	CHECK_INCLUDE_FILES_CONCAT("netinet/in.h"    HAVE_NETINET_IN_H)
	CHECK_INCLUDE_FILES_CONCAT("pwd.h"           HAVE_PWD_H)
	CHECK_INCLUDE_FILES_CONCAT("sgtty.h"         HAVE_SGTTY_H)
	CHECK_INCLUDE_FILES_CONCAT("stdint.h"        HAVE_STDINT_H)
	CHECK_INCLUDE_FILES_CONCAT("stdlib.h"        HAVE_STDLIB_H)
	CHECK_INCLUDE_FILES_CONCAT("string.h"        HAVE_STRING_H)
	CHECK_INCLUDE_FILES_CONCAT("strings.h"       HAVE_STRINGS_H)
	CHECK_INCLUDE_FILES_CONCAT("termios.h"       HAVE_TERMIOS_H)
	CHECK_INCLUDE_FILES_CONCAT("termio.h"        HAVE_TERMIO_H)
	CHECK_INCLUDE_FILES_CONCAT("io.h"            HAVE_IO_H)
	CHECK_INCLUDE_FILES_CONCAT("time.h"          HAVE_TIME_H)
	CHECK_INCLUDE_FILES_CONCAT("unistd.h"        HAVE_UNISTD_H)
	CHECK_INCLUDE_FILES_CONCAT("sockio.h"        HAVE_SOCKIO_H)
	CHECK_INCLUDE_FILES_CONCAT("x509.h"          HAVE_X509_H)
	CHECK_INCLUDE_FILES_CONCAT("setjmp.h"        HAVE_SETJMP_H)
	CHECK_INCLUDE_FILES_CONCAT("signal.h"        HAVE_SIGNAL_H)
	CHECK_INCLUDE_FILES_CONCAT("pthread.h"   	 HAVE_PTHREAD_H)
	CHECK_INCLUDE_FILES_CONCAT("ansidecl.h"   	 HAVE_ANSIDECL_H)
	CHECK_INCLUDE_FILES_CONCAT("ctype.h"  		 HAVE_CTYPE_H)
	CHECK_INCLUDE_FILES_CONCAT("dirent.h"  		 HAVE_DIRENT_H)
	CHECK_INCLUDE_FILES_CONCAT("dl.h"  		 	 HAVE_DL_H)
	CHECK_INCLUDE_FILES_CONCAT("errno.h"  		 HAVE_ERRNO_H)
	CHECK_INCLUDE_FILES_CONCAT("float.h"  		 HAVE_FLOAT_H)
	CHECK_INCLUDE_FILES_CONCAT("fp_class.h"  	 HAVE_FP_CLASS_H)
	CHECK_INCLUDE_FILES_CONCAT("resolv.h"  	 	 HAVE_RESOLV_H)
	CHECK_INCLUDE_FILES_CONCAT("stdarg.h"  	 	 HAVE_STDARG_H)
	#
	# Win32
	#
	CHECK_INCLUDE_FILES_CONCAT("windows.h"		 HAVE_WINDOWS_H)
	CHECK_INCLUDE_FILES_CONCAT("winsock.h"       HAVE_WINSOCK_H)
	CHECK_INCLUDE_FILES_CONCAT("winsock2.h"      HAVE_WINSOCK2_H)
	CHECK_INCLUDE_FILES_CONCAT("ws2tcpip.h"      HAVE_WS2TCPIP_H)
	#
	# POSIX
	#
	CHECK_INCLUDE_FILES_CONCAT("sys/dir.h"       HAVE_SYS_DIR_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/ioctl.h"     HAVE_SYS_IOCTL_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/mman.h"      HAVE_SYS_MMAN_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/ndir.h"      HAVE_SYS_NDIR_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/param.h"     HAVE_SYS_PARAM_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/poll.h"      HAVE_SYS_POLL_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/select.h"    HAVE_SYS_SELECT_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/socket.h"    HAVE_SYS_SOCKET_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/sockio.h"    HAVE_SYS_SOCKIO_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/stat.h"      HAVE_SYS_STAT_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/time.h"      HAVE_SYS_TIME_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/timeb.h"     HAVE_SYS_TIMEB_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/types.h"     HAVE_SYS_TYPES_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/utime.h"     HAVE_SYS_UTIME_H)
	CHECK_INCLUDE_FILES_CONCAT("sys/utsname.h"   HAVE_SYS_UTSNAME_H)

	CHECK_TYPE_SIZE(size_t  SIZEOF_SIZE_T)
	CHECK_TYPE_SIZE(ssize_t  SIZEOF_SSIZE_T)
	CHECK_TYPE_SIZE("long long"  SIZEOF_LONG_LONG)
	CHECK_TYPE_SIZE("long double"  SIZEOF_LONG_DOUBLE)
	CHECK_TYPE_SIZE(socklen_t  XML_SOCKLEN_T)

	if( WIN32 )
		set( in_addr_t "unsigned long" CACHE INTERNAL "in_addr_t" )
	endif( WIN32 )
	IF(NOT HAVE_SIZEOF_SSIZE_T)
	  SET(ssize_t int)
	ENDIF(NOT HAVE_SIZEOF_SSIZE_T)
	IF(HAVE_SIZEOF_LONG_LONG)
	  SET(HAVE_LONGLONG 1)
	ENDIF(HAVE_SIZEOF_LONG_LONG)

	FIND_FILE(RANDOM_FILE urandom /dev)
	MARK_AS_ADVANCED(RANDOM_FILE)

	# Check for some functions that are used
	CHECK_SYMBOL_EXISTS(socket        "${REQUIRED_INCLUDES}" HAVE_SOCKET)
	CHECK_SYMBOL_EXISTS(poll          "${REQUIRED_INCLUDES}" HAVE_POLL)
	CHECK_SYMBOL_EXISTS(select        "${REQUIRED_INCLUDES}" HAVE_SELECT)
	CHECK_SYMBOL_EXISTS(strdup        "${REQUIRED_INCLUDES}" HAVE_STRDUP)
	CHECK_SYMBOL_EXISTS(strstr        "${REQUIRED_INCLUDES}" HAVE_STRSTR)
	CHECK_SYMBOL_EXISTS(strtok_r      "${REQUIRED_INCLUDES}" HAVE_STRTOK_R)
	CHECK_SYMBOL_EXISTS(strftime      "${REQUIRED_INCLUDES}" HAVE_STRFTIME)
	CHECK_SYMBOL_EXISTS(uname         "${REQUIRED_INCLUDES}" HAVE_UNAME)
	CHECK_SYMBOL_EXISTS(strcasecmp    "${REQUIRED_INCLUDES}" HAVE_STRCASECMP)
	CHECK_SYMBOL_EXISTS(stricmp       "${REQUIRED_INCLUDES}" HAVE_STRICMP)
	CHECK_SYMBOL_EXISTS(strcmpi       "${REQUIRED_INCLUDES}" HAVE_STRCMPI)
	CHECK_SYMBOL_EXISTS(strncmpi      "${REQUIRED_INCLUDES}" HAVE_STRNCMPI)
	#	IF(NOT HAVE_STRNCMPI)
	#	  SET(HAVE_STRCMPI)
	#	ENDIF(NOT HAVE_STRNCMPI)
	CHECK_SYMBOL_EXISTS(gethostbyaddr "${REQUIRED_INCLUDES}" HAVE_GETHOSTBYADDR)
	CHECK_SYMBOL_EXISTS(gettimeofday  "${REQUIRED_INCLUDES}" HAVE_GETTIMEOFDAY)
	CHECK_SYMBOL_EXISTS(inet_addr     "${REQUIRED_INCLUDES}" HAVE_INET_ADDR)
	CHECK_SYMBOL_EXISTS(inet_pton     "${REQUIRED_INCLUDES}" HAVE_INET_PTON)
	CHECK_SYMBOL_EXISTS(inet_ntoa     "${REQUIRED_INCLUDES}" HAVE_INET_NTOA)
	CHECK_SYMBOL_EXISTS(inet_ntoa_r   "${REQUIRED_INCLUDES}" HAVE_INET_NTOA_R)
	CHECK_SYMBOL_EXISTS(tcsetattr     "${REQUIRED_INCLUDES}" HAVE_TCSETATTR)
	CHECK_SYMBOL_EXISTS(tcgetattr     "${REQUIRED_INCLUDES}" HAVE_TCGETATTR)
	CHECK_SYMBOL_EXISTS(perror        "${REQUIRED_INCLUDES}" HAVE_PERROR)
	CHECK_SYMBOL_EXISTS(closesocket   "${REQUIRED_INCLUDES}" HAVE_CLOSESOCKET)
	CHECK_SYMBOL_EXISTS(setvbuf       "${REQUIRED_INCLUDES}" HAVE_SETVBUF)
	CHECK_SYMBOL_EXISTS(sigsetjmp     "${REQUIRED_INCLUDES}" HAVE_SIGSETJMP)
	CHECK_SYMBOL_EXISTS(getpass_r     "${REQUIRED_INCLUDES}" HAVE_GETPASS_R)
	CHECK_SYMBOL_EXISTS(strlcat       "${REQUIRED_INCLUDES}" HAVE_STRLCAT)
	CHECK_SYMBOL_EXISTS(getpwuid      "${REQUIRED_INCLUDES}" HAVE_GETPWUID)
	CHECK_SYMBOL_EXISTS(geteuid       "${REQUIRED_INCLUDES}" HAVE_GETEUID)
	CHECK_SYMBOL_EXISTS(utime         "${REQUIRED_INCLUDES}" HAVE_UTIME)
	IF(CMAKE_USE_OPENSSL)
	  CHECK_SYMBOL_EXISTS(RAND_status   "${REQUIRED_INCLUDES}" HAVE_RAND_STATUS)
	  CHECK_SYMBOL_EXISTS(RAND_screen   "${REQUIRED_INCLUDES}" HAVE_RAND_SCREEN)
	  CHECK_SYMBOL_EXISTS(RAND_egd      "${REQUIRED_INCLUDES}" HAVE_RAND_EGD)
	  CHECK_SYMBOL_EXISTS(CRYPTO_cleanup_all_ex_data "${REQUIRED_INCLUDES}"
		HAVE_CRYPTO_CLEANUP_ALL_EX_DATA)
	ENDIF(CMAKE_USE_OPENSSL)
	CHECK_SYMBOL_EXISTS(gmtime_r      "${REQUIRED_INCLUDES}" HAVE_GMTIME_R)
	CHECK_SYMBOL_EXISTS(localtime_r   "${REQUIRED_INCLUDES}" HAVE_LOCALTIME_R)

	CHECK_SYMBOL_EXISTS(gethostbyname   "${REQUIRED_INCLUDES}" HAVE_GETHOSTBYNAME)
	CHECK_SYMBOL_EXISTS(gethostbyname_r "${REQUIRED_INCLUDES}" HAVE_GETHOSTBYNAME_R)
	CHECK_SYMBOL_EXISTS(gethostbyaddr_r "${REQUIRED_INCLUDES}" HAVE_GETHOSTBYADDR_R)

	CHECK_SYMBOL_EXISTS(signal        "${REQUIRED_INCLUDES}" HAVE_SIGNAL_FUNC)
	CHECK_SYMBOL_EXISTS(SIGALRM       "${REQUIRED_INCLUDES}" HAVE_SIGNAL_MACRO)
	IF(HAVE_SIGNAL_FUNC AND HAVE_SIGNAL_MACRO)
	  SET(HAVE_SIGNAL 1)
	ENDIF(HAVE_SIGNAL_FUNC AND HAVE_SIGNAL_MACRO)
	CHECK_SYMBOL_EXISTS(uname         "${REQUIRED_INCLUDES}" HAVE_UNAME)
	CHECK_SYMBOL_EXISTS(strtoll       "${REQUIRED_INCLUDES}" HAVE_STRTOLL)
	CHECK_SYMBOL_EXISTS(_strtoi64     "${REQUIRED_INCLUDES}" HAVE__STRTOI64)
	CHECK_SYMBOL_EXISTS(strerror_r    "${REQUIRED_INCLUDES}" HAVE_STRERROR_R)
	CHECK_SYMBOL_EXISTS(strerror      "${REQUIRED_INCLUDES}" HAVE_STRERROR)
	CHECK_SYMBOL_EXISTS(siginterrupt  "${REQUIRED_INCLUDES}" HAVE_SIGINTERRUPT)
	CHECK_SYMBOL_EXISTS(perror        "${REQUIRED_INCLUDES}" HAVE_PERROR)
	CHECK_SYMBOL_EXISTS(getaddinfo    "${REQUIRED_INCLUDES}" HAVE_GETADDRINFO)
	CHECK_SYMBOL_EXISTS(gettimeofday  "${REQUIRED_INCLUDES}" HAVE_GETTIMEOFDAY)
	CHECK_SYMBOL_EXISTS(snprintf      "${REQUIRED_INCLUDES}" HAVE_SNPRINTF)
	CHECK_SYMBOL_EXISTS(sprintf       "${REQUIRED_INCLUDES}" HAVE_SPRINTF)
	CHECK_SYMBOL_EXISTS(asprintf      "${REQUIRED_INCLUDES}" HAVE_ASPRINTF)
	CHECK_SYMBOL_EXISTS(wprintf       "${REQUIRED_INCLUDES}" HAVE_WPRINTF)
	CHECK_SYMBOL_EXISTS(stat 	      "${REQUIRED_INCLUDES}" HAVE_STAT)
	CHECK_SYMBOL_EXISTS(_stat 	      "${REQUIRED_INCLUDES}" HAVE__STAT)
	CHECK_SYMBOL_EXISTS(strndup 	  string.h HAVE_STRNDUP)
	CHECK_SYMBOL_EXISTS(stpcpy 	  	  string.h HAVE_STPCPY)
	CHECK_SYMBOL_EXISTS(mempcpy 	  string.h HAVE_MEMPCPY)
	#
	set( HAVE_POSIX_PRINTF 0 CACHE INTERNAL "Posix printf (whatever that is" )
	#
	if( NOT HAVE_ASPRINTF )
		set( HAVE_ASPRINTF 0 CACHE INTERNAL "Lame!" )
	endif( NOT HAVE_ASPRINTF )
	#
	if( NOT HAVE_WPRINTF )
		set( HAVE_WPRINTF 0 CACHE INTERNAL "Lame!!!!!" )
	endif( NOT HAVE_WPRINTF )
	#
	CHECK_SYMBOL_EXISTS(va_copy 	  "${REQUIRED_INCLUDES}" HAVE_VA_COPY)
	CHECK_SYMBOL_EXISTS(__va_copy 	  "${REQUIRED_INCLUDES}" HAVE___VA_COPY)
	CHECK_SYMBOL_EXISTS(vfprintf 	  "${REQUIRED_INCLUDES}" HAVE_VFPRINTF)
	CHECK_SYMBOL_EXISTS(vsnprintf 	  "${REQUIRED_INCLUDES}" HAVE_VSNPRINTF)
	CHECK_SYMBOL_EXISTS(vsprintf 	  "${REQUIRED_INCLUDES}" HAVE_VSPRINTF)
	CHECK_SYMBOL_EXISTS(readlink 	  "${REQUIRED_INCLUDES}" HAVE_READLINK)
	CHECK_SYMBOL_EXISTS(basename 	  "${REQUIRED_INCLUDES}" HAVE_BASENAME)

	if( HAVE_SYS_SOCKET_H OR HAVE_WINSOCK_H )
		set( HAVE_RECV 1 CACHE INTERNAL "recv() exists on system" )
		set( HAVE_SEND 1 CACHE INTERNAL "send() exists on system" )
		if( WIN32 )
			set( RECV_TYPE_ARG1 SOCKET		CACHE INTERNAL "argument for recv/send functions" )
			set( RECV_TYPE_ARG2 "char *"	CACHE INTERNAL "argument for recv/send functions" )
			set( RECV_TYPE_ARG3 int			CACHE INTERNAL "argument for recv/send functions" )
			set( RECV_TYPE_ARG4 int			CACHE INTERNAL "argument for recv/send functions" )
			set( RECV_TYPE_RETV int			CACHE INTERNAL "argument for recv/send functions" )
			#                            
			set( SEND_TYPE_ARG1 SOCKET		CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_QUAL_ARG2 const		CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_TYPE_ARG2 "char *"	CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_TYPE_ARG3 int			CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_TYPE_ARG4 int			CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_TYPE_RETV int			CACHE INTERNAL "argument for recv/send functions" )
		else( WIN32 )                    
			set( RECV_TYPE_ARG1 int			CACHE INTERNAL "argument for recv/send functions" )
			set( RECV_TYPE_ARG2 "char *"	CACHE INTERNAL "argument for recv/send functions" )
			set( RECV_TYPE_ARG3 size_t		CACHE INTERNAL "argument for recv/send functions" )
			set( RECV_TYPE_ARG4 int			CACHE INTERNAL "argument for recv/send functions" )
			set( RECV_TYPE_RETV ssize_t		CACHE INTERNAL "argument for recv/send functions" )
			#                            
			set( SEND_TYPE_ARG1 int			CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_QUAL_ARG2 const		CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_TYPE_ARG2 "char *"	CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_TYPE_ARG3 size_t		CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_TYPE_ARG4 int			CACHE INTERNAL "argument for recv/send functions" )
			set( SEND_TYPE_RETV ssize_t		CACHE INTERNAL "argument for recv/send functions" )
		endif( WIN32 )
	endif( HAVE_SYS_SOCKET_H OR HAVE_WINSOCK_H )

	IF(WINDOWS)
		SET(EXTRA_DEFINES ${EXTRA_DEFINES}
				"__unused7\n#ifndef WIN32_LEAN_AND_MEAN\n#define WIN32_LEAN_AND_MEAN\n#endif\n#define __unused3")
		add_header_include(HAVE_WINDOWS_H "windows.h")
		add_header_include(HAVE_WINSOCK_H "winsock.h")
		add_header_include(HAVE_WINSOCK2_H "winsock2.h")
		add_header_include(HAVE_WS2TCPIP_H "ws2tcpip.h")
	ELSE(WINDOWS)
		add_header_include(HAVE_SYS_TYPES_H "sys/types.h")
		add_header_include(HAVE_SYS_TIME_H "sys/time.h")
		add_header_include(TIME_WITH_SYS_TIME "time.h")
		add_header_include(HAVE_TIME_H "time.h")
	ENDIF(WINDOWS)
	MO_CHECK_C_SOURCE_COMPILES("struct timeval ts;\nts.tv_sec  = 0;\nts.tv_usec = 0" HAVE_STRUCT_TIMEVAL)

	CHECK_FUNCTION_EXISTS( ioctlsocket HAVE_IOCTLSOCKET )
	CHECK_FUNCTION_EXISTS( Ioctlsocket HAVE_IOCTLSOCKET_CASE )
	if( HAVE_WINSOCK_H )
		set( HAVE_IOCTLSOCKET 1 CACHE INTERNAL "Winsock version of ioctlsocket" )
	endif( HAVE_WINSOCK_H )

	CHECK_FUNCTION_EXISTS( fcntl HAVE_FCNTL )
	if( HAVE_FCNTL )
		set( HAVE_O_NONBLOCK 1 CACHE INTERNAL "We found fcntl so we can block, and we're no doubt on Linux" )
	endif( HAVE_FCNTL )

	set( HEADER_INCLUDES "errno.h" )
	#
	MO_CHECK_C_SOURCE_COMPILES( "char buffer[1024];
			char *string = strerror_r(EACCES, buffer, sizeof(buffer));
			if(!string || !string[0])
				return 99;
				return 0;"
		HAVE_GLIBC_STRERROR_R )

	MO_CHECK_C_SOURCE_COMPILES( "char buffer[1024];
			int error = strerror_r(EACCES, buffer, sizeof(buffer));
			if(!buffer[0] || error)
				return 99;
				return 0;"
		HAVE_POSIX_STRERROR_R )

	MO_CHECK_C_SOURCE_COMPILES( "extern __attribute__((__visibility__("hidden"))) int hiddenvar;
			extern __attribute__((__visibility__("default"))) int exportedvar;
			extern __attribute__((__visibility__("hidden"))) int hiddenfunc (void);
			extern __attribute__((__visibility__("default"))) int exportedfunc (void);"
		HAVE_VISIBILITY )

    set( HEADER_INCLUDES "#include <stdlib.h>
		#include <iconv.h>" )
	MO_CHECK_C_SOURCE_COMPILES( "extern
			#ifdef __cplusplus
			\"C\"
			#endif
			#if defined(__STDC__) || defined(__cplusplus)
			size_t iconv (iconv_t cd, char * *inbuf, size_t *inbytesleft, char * *outbuf, size_t *outbytesleft);
			#else
			size_t iconv();
			#endif"
		ICONV_NEEDS_CONST )
	#
	if( ICONV_NEEDS_CONST )
		set( ICONV_CONST_VALUE "const" )
	else( ICONV_NEEDS_CONST )
		set( ICONV_CONST_VALUE " " )
	endif( ICONV_NEEDS_CONST )
	#
	set( ICONV_CONST "${ICONV_CONST_VALUE}" CACHE INTERNAL "Set to const if ICONV needs this in the decl" )

	MO_CHECK_C_SOURCE_COMPILES( "wchar_t foo = (wchar_t)'\0';"
		NEEDS_WCHAR_T )
	#
	if( NEEDS_WCHAR_T )
		set( HAVE_WCHAR_T 1 CACHE INTERNAL "Set if compiler supports wchar_t" )
	else( NEEDS_WCHAR_T )
		set( HAVE_WCHAR_T 0 CACHE INTERNAL "Set if compiler supports wchar_t" )
	endif( NEEDS_WCHAR_T )

	# Check for ipv6 and other socket-related stuff
	#
    set( HEADER_INCLUDES "
		#include <sys/types.h>
		#include <sys/socket.h>"
		)
	#
	MO_CHECK_C_SOURCE_COMPILES("
		struct sockaddr_storage ss;
		socket(AF_INET6, SOCK_STREAM, 0);"
		HAS_IPV6
		)
	#
	if( HAS_IPV6 )
		set( SUPPORT_IP6 1 CACHE INTERNAL "We have IPV6 support" )
	endif( HAS_IPV6 )
	#
	# On some platforms (like AIX 5L), the structure sockaddr doesn't have
	# a ss_family member, but rather __ss_family. Let's detect that
	# and define the HAVE_BROKEN_SS_FAMILY when we are on one of these
	# platforms.  However, we should only do this if ss_family is not
	# present.
	#
	MO_CHECK_C_SOURCE_COMPILES("
	  struct sockaddr_storage ss;
      ss.ss_family = 0;"
	  HAS_SS_FAMILY
	  )
	#
	if( HAS_SS_FAMILY )
		set( HAVE_SS_FAMILY 1 CACHE INTERNAL "We have ss_family support" )
		set( ss_family ss_family CACHE INTERNAL "Using broken ss_family" )
	endif( HAS_SS_FAMILY )
	#
	MO_CHECK_C_SOURCE_COMPILES("
	  struct sockaddr_storage ss;
	  ss.__ss_family = 0;"
	  HAS_BROKEN_SS_FAMILY
	  )
	#
	if( HAS_BROKEN_SS_FAMILY )
		set( HAVE_BROKEN_SS_FAMILY 1 CACHE INTERNAL "ss_family is supported, but it's the broken one." )
		set( ss_family __ss_family CACHE INTERNAL "Using broken ss_family" )
	endif( HAS_BROKEN_SS_FAMILY )

	if( HAVE_VISIBILITY )
		set( CFLAG_VISIBILITY "-fvisibility=hidden" )
	endif( HAVE_VISIBILITY )

	# TODO: make this crossplatform!
	#
	set( LOCALEDIR "\"/usr/share/locale\"" CACHE INTERNAL "Locale directory--this is system dependent!" )

	# The following libraries are provided this in the sandbox, so we can force all these on
	#
	set( HAVE_IDN_FREE_H	1 CACHE INTERNAL "idn-free header"     )
	set( HAVE_IDN_FREE		1 CACHE INTERNAL "idn-free function"   )
	set( HAVE_LIBIDN		1 CACHE INTERNAL "libidn library"      )
	set( HAVE_LIBZ			1 CACHE INTERNAL "libz library"        )
	set( STDC_HEADERS		1 CACHE INTERNAL "using std-c headers" )
	set( HAVE_PROTOTYPES	1 CACHE INTERNAL "Our compilers can cope with arguments to functions" )


	# System dependent definitions
	#
	if( LINUX AND DARWIN )
	    set(HAVE_ISINF	1 CACHE INTERNAL "ISINF Defined" )
        set(HAVE_ISINFF	1 CACHE INTERNAL "ISINFF Defined" )
        set(HAVE_ISNAN	1 CACHE INTERNAL "ISNAN Defined" )
        set(HAVE_ISNANF	1 CACHE INTERNAL "ISNANF Defined" )
	else ( LINUX AND DARWIN )
		if( WINDOWS )
			set( _WINSOCKAPI_	1			CACHE INTERNAL	"Using winsock" )
			set( snprintf		_snprintf	CACHE INTERNAL	"Win32 has its own version" )
			set( vsnprintf		_vsnprintf	CACHE INTERNAL	"Win32 has its own version" )
		endif( WINDOWS )
	    set(HAVE_ISINF	0 CACHE INTERNAL "ISINF Defined" )
        set(HAVE_ISINFF	0 CACHE INTERNAL "ISINFF Defined" )
        set(HAVE_ISNAN	0 CACHE INTERNAL "ISNAN Defined" )
        set(HAVE_ISNANF	0 CACHE INTERNAL "ISNANF Defined" )
	endif( LINUX AND DARWIN )

endfunction( ConfigureSystemIncludes )

# vim: ts=4 sw=4 noexpandtab
