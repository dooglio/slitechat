# -*- cmake -*-

project(llprimitive)

include(00-Common)
include(APR)
include(Boost)
include(EXPAT)
include(ZLIB)

include_directories(
	${LLC_SOURCE_DIR}/llcommon
	${LLC_SOURCE_DIR}/llmath
	${LLC_SOURCE_DIR}/llmessage
	${LLC_SOURCE_DIR}/llxml
    ${APR_INCLUDE_DIR}
    ${APRUTIL_INCLUDE_DIR}
    ${EXPAT_INCLUDE_DIRS}
    ${BOOST_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    )

set(llprimitive_SOURCE_FILES
    llmaterialtable.cpp
    llprimitive.cpp
    lltextureanim.cpp
    lltextureentry.cpp
    lltreeparams.cpp
    llvolumemessage.cpp
    llvolumexml.cpp
    )

set(llprimitive_HEADER_FILES
    legacy_object_types.h
    llmaterialtable.h
    llprimitive.h
    lltextureanim.h
    lltextureentry.h
    lltreeparams.h
    lltree_common.h
    llvolumemessage.h
    llvolumexml.h
    material_codes.h
    object_flags.h
    )
        
set_source_files_properties(${llprimitive_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llprimitive_SOURCE_FILES ${llprimitive_HEADER_FILES})

add_library (llprimitive ${llprimitive_SOURCE_FILES})
