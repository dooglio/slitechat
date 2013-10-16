# -*- cmake -*-

project(llcharacter)

include(00-Common)
include(APR)
include(Boost)
include(EXPAT)
include(ZLIB)

include_directories(
	${LLC_SOURCE_DIR}/llcommon
	${LLC_SOURCE_DIR}/llinventory
	${LLC_SOURCE_DIR}/llmath
	${LLC_SOURCE_DIR}/llmessage
	${LLC_SOURCE_DIR}/llxml
	${LLC_SOURCE_DIR}/llvfs
    ${APR_INCLUDE_DIR}
    ${APRUTIL_INCLUDE_DIR}
    ${EXPAT_INCLUDE_DIRS}
    ${BOOST_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    )

set(llcharacter_SOURCE_FILES
    llanimationstates.cpp
    llbvhloader.cpp
    llcharacter.cpp
    lleditingmotion.cpp
    llgesture.cpp
    llhandmotion.cpp
    llheadrotmotion.cpp
    lljoint.cpp
    lljointsolverrp3.cpp
    llkeyframefallmotion.cpp
    llkeyframemotion.cpp
    llkeyframemotionparam.cpp
    llkeyframestandmotion.cpp
    llkeyframewalkmotion.cpp
    llmotioncontroller.cpp
    llmotion.cpp
    llmultigesture.cpp
    llpose.cpp
    llstatemachine.cpp
    lltargetingmotion.cpp
    llvisualparam.cpp
    )

set(llcharacter_HEADER_FILES
    llanimationstates.h
    llbvhloader.h
    llcharacter.h
    lleditingmotion.h
    llgesture.h
    llhandmotion.h
    llheadrotmotion.h
    lljoint.h
    lljointsolverrp3.h
    lljointstate.h
    llkeyframefallmotion.h
    llkeyframemotion.h
    llkeyframemotionparam.h
    llkeyframestandmotion.h
    llkeyframewalkmotion.h
    llmotion.h
    llmotioncontroller.h
    llmultigesture.h
    llpose.h
    llstatemachine.h
    lltargetingmotion.h
    llvisualparam.h
    )

set_source_files_properties(${llcharacter_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llcharacter_SOURCE_FILES ${llcharacter_HEADER_FILES})

add_library (llcharacter ${llcharacter_SOURCE_FILES})
