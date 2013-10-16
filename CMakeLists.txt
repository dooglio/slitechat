cmake_minimum_required (VERSION 2.6)
project(SLiteChat)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )

option( BUILD_SLITECHAT "Build the SLiteChat project" TRUE )
option( BUILD_XGRIDCHAT "Build the XGridChat project" TRUE )

add_subdirectory(LLChatLib)

if( BUILD_SLITECHAT )
	add_subdirectory(SLiteChat)
endif( BUILD_SLITECHAT )

if( BUILD_XGRIDCHAT )
	add_subdirectory(XGridChat)
endif( BUILD_XGRIDCHAT )

# vim: ts=4 sw=4 noexpandtab
