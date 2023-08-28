cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(remote-cinema-client)

add_executable(remote-cinema-client)
set(qt Qt5)

set(sources
	src/main.cpp
        src/mainwindow.h
        src/mainwindow.cpp
        src/mainwindow.ui
        src/imageviewwidget.h
	)

find_package(${qt} COMPONENTS Core Widgets REQUIRED)

target_sources(remote-cinema-client PRIVATE ${sources})

target_link_libraries(remote-cinema-client PRIVATE ${qt}::Widgets avcodec avutil)

target_include_directories(remote-cinema-client PRIVATE /usr/include/ffmpeg)

set_target_properties(remote-cinema-client PROPERTIES
	DEBUG_POSTFIX d 
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
	)

