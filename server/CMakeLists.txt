﻿cmake_policy(SET CMP0048 NEW)
cmake_minimum_required (VERSION 3.8)
project(server)

add_library (server STATIC
		"src/server/connection/Socket.cpp"
		"src/server/ClientRegistry.cpp"
		"src/server/Server.cpp"
		"src/server/ServerBuilder.cpp"
		"src/server/throw.cpp")

target_compile_features(server PUBLIC cxx_std_17)
target_include_directories(server PUBLIC "include")
target_include_directories(server PRIVATE "src")

install(DIRECTORY include
		DESTINATION include
		FILES_MATCHING PATTERN "*.hpp")
install(TARGETS server
		DESTINATION lib)
