cmake_minimum_required (VERSION 3.8)

if(MSVC)
	add_compile_options(/W3)
endif()

add_executable (machiavelli
	"src/main.cpp"
	"src/machiavelli/Game.cpp"
)

target_compile_features(machiavelli PUBLIC cxx_std_17)
target_link_libraries(machiavelli PRIVATE server)
target_include_directories(machiavelli PRIVATE 
	"src"
	"include"
)
