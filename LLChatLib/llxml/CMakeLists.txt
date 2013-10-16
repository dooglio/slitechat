# -*- cmake -*-

project(llxml)

set( WANT_UNICODE true )

include(00-Common)
include(APR)
include(Boost)
include(EXPAT)
include(ZLIB)

include_directories(
	${LLC_SOURCE_DIR}/llcommon
	${LLC_SOURCE_DIR}/llmath
	${LLC_SOURCE_DIR}/llxml
    ${APR_INCLUDE_DIR}
    ${APRUTIL_INCLUDE_DIR}
    ${EXPAT_INCLUDE_DIRS}
    ${BOOST_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    )

set(llxml_SOURCE_FILES
    llcontrol.cpp
    llxmlnode.cpp
    llxmlparser.cpp
    llxmltree.cpp
    )

set(llxml_HEADER_FILES
    llcontrol.h
    llxmlnode.h
    llxmlparser.h
    llxmltree.h
    )

set_source_files_properties(${llxml_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llxml_SOURCE_FILES ${llxml_HEADER_FILES})

add_library (llxml STATIC ${llxml_SOURCE_FILES})
target_link_libraries(
    llxml
    ${BOOST_SIGNALS_LIBRARY}
    ${EXPAT_LIBRARIES}
    )
