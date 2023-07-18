cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

project(remote-cinema-server)

add_executable(remote-cinema-server)

set(sources 
	src/main.cpp
	)

target_sources(remote-cinema-server PRIVATE ${sources})

target_link_libraries(remote-cinema-server PRIVATE avcodec avutil)
target_compile_definitions(remote-cinema-server PUBLIC $<$<CONFIG:DEBUG>:DEBUG>)
if(UNIX)
target_include_directories(remote-cinema-server 
	PRIVATE
		/usr/include/ffmpeg
	)
endif()

set_target_properties(remote-cinema-server PROPERTIES
	DEBUG_POSTFIX d 
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
	)
