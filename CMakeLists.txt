cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.8)

set(INSTALL_DIR ${PROJECT_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${INSTALL_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${INSTALL_DIR})

set(CMAKE_CXX_FLAGS -pthread)
add_subdirectory(server)
add_subdirectory(machiavelli)