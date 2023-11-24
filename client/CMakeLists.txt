cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(remote-cinema-client)

add_executable(remote-cinema-client)
set(qt Qt5)

set(sources
	src/main.cpp
        src/mainwindow.h
        src/mainwindow.cpp
        src/mainwindow.ui
        src/imageviewwidget.h
        src/imageviewwidget.cpp
        #src/decode.c
        #src/encode.c
	)

find_package(${qt} COMPONENTS Core Widgets REQUIRED)
find_library(AVCODEC_LIBRARY avcodec)
find_library(AVFORMAT_LIBRARY avformat)

target_sources(remote-cinema-client PRIVATE ${sources})

#target_link_libraries(remote-cinema-client PRIVATE ${qt}::Widgets libavcodec libavformat libavdevice libavfilter libswscale libswresample)
target_link_libraries(remote-cinema-client PRIVATE ${qt}::Widgets ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY})

target_include_directories(remote-cinema-client PRIVATE /usr/include/ffmpeg)

set_target_properties(remote-cinema-client PROPERTIES
	DEBUG_POSTFIX d 
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
	)

