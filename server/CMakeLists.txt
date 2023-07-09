cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

project(remote-cinema-server)

add_executable(remote-cinema-server)

set(sources 
	src/main.cpp
	)

target_sources(remote-cinema-server PRIVATE ${sources})
