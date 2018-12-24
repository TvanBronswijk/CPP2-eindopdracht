cmake_policy(SET CMP0048 NEW)
cmake_minimum_required (VERSION 3.8)
project(server)

if(MSVC)
	add_compile_options(/W3)
endif(MSVC)

add_library (server STATIC
	"src/server/Player.cpp"
	"src/server/Socket.cpp"
	"src/server/throw.cpp"
	"src/server/Server.cpp"
)

target_compile_features(server PUBLIC cxx_std_14)
target_include_directories(server PUBLIC "include")
target_include_directories(server PRIVATE "src")

install(DIRECTORY include/server
	DESTINATION include/server
	FILES_MATCHING PATTERN "*.hpp"
)
install(TARGETS server
	DESTINATION lib
)
