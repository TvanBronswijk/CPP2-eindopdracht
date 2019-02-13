cmake_minimum_required (VERSION 3.8)

add_executable (machiavelli
		"src/main.cpp"
		"src/machiavelli/InputHandler.cpp"
		"src/machiavelli/Game.cpp"
		"src/machiavelli/states/BaseState.cpp"
		"src/machiavelli/states/LobbyState.cpp"
		"src/machiavelli/states/IdleState.cpp"
		"src/machiavelli/states/ActionState.cpp"
		"src/machiavelli/states/OptionState.cpp"
		"src/machiavelli/states/CharacterCardPickingState.cpp")

target_compile_features(machiavelli PUBLIC cxx_std_17)
target_link_libraries(machiavelli PRIVATE server)
target_include_directories(machiavelli PRIVATE
		"src"
		"include")

add_custom_target(machiavelli-resources ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/layouts" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/layouts")