cmake_minimum_required (VERSION 3.21)
project (bittool)
set (CMAKE_CXX_STANDARD 20)

file(GLOB bittool_SRC "codec/*.c")
add_executable(bittool bittool.cpp ${bittool_SRC} )
target_link_libraries(bittool PRIVATE fec)
