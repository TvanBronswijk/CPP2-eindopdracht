cmake_minimum_required (VERSION 3.8)

if(MSVC)
	add_compile_options(/W3)
endif()

add_executable (machiavelli
	"src/main.cpp"
	"src/machiavelli/Game.cpp"
	"src/machiavelli/parser/parser.cpp"
)

target_compile_features(machiavelli PUBLIC cxx_std_17)
target_link_libraries(machiavelli PRIVATE server)
target_include_directories(machiavelli PRIVATE 
	"src"
	"include"
)

add_custom_target(machiavelli-resources ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/layouts" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/layouts")