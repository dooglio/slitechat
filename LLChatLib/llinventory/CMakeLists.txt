# -*- cmake -*-

project(llinventory)

set( WANT_UNICODE true )

include(00-Common)
include(APR)
include(Boost)
include(EXPAT)
include(ZLIB)
include(CARes)
include(CURL)
include(OpenSSL)
include(XmlRpcEpi)

include_directories(
	${LLC_SOURCE_DIR}/llcommon
	${LLC_SOURCE_DIR}/llinventory
	${LLC_SOURCE_DIR}/llmath
	${LLC_SOURCE_DIR}/llmessage
	${LLC_SOURCE_DIR}/llxml
    ${APRUTIL_INCLUDE_DIR}
    ${APR_INCLUDE_DIR}
    ${BOOST_INCLUDE_DIRS}
    ${CARES_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
    )

set(llinventory_SOURCE_FILES
    llcategory.cpp
    lleconomy.cpp
    llinventory.cpp
    llinventorytype.cpp
    lllandmark.cpp
    llnotecard.cpp
    llparcel.cpp
    llpermissions.cpp
    llsaleinfo.cpp
    lltransactionflags.cpp
    lluserrelations.cpp
    )
    
set(llinventory_HEADER_FILES
    llcategory.h
    lleconomy.h
    llinventory.h
    llinventorytype.h
    lllandmark.h
    llnotecard.h
    llparcel.h
    llparcelflags.h
    llpermissions.h
    llpermissionsflags.h
    llsaleinfo.h
    lltransactionflags.h
    lltransactiontypes.h
    lluserrelations.h
    )

set_source_files_properties(${llinventory_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llinventory_SOURCE_FILES ${llinventory_HEADER_FILES})

add_library (llinventory STATIC ${llinventory_SOURCE_FILES})
