/* lib/config.h.cmake.  Use this to create a config.h automaticall with CMake.  */

/* Define to 1 if you have the <arpa/inet.h> header file. */
#cmakedefine HAVE_ARPA_INET_H ${HAVE_ARPA_INET_H}

/* Define to 1 if you have the <netinet/in.h> header file. */
#cmakedefine HAVE_NETINET_IN_H ${HAVE_NETINET_IN_H}

/* For windows support */
#cmakedefine HAVE_WINDOWS_H ${HAVE_WINDOWS_H}
#cmakedefine HAVE_WINSOCK_H ${HAVE_WINSOCK_H}
#cmakedefine HAVE_WINSOCK2_H ${HAVE_WINSOCK2_H}
#cmakedefine HAVE_WS2TCPIP_H ${HAVE_WS2TCPIP_H}
