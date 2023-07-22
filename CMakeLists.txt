cmake_minimum_required (VERSION 3.21)
project (bittool)
set (CMAKE_CXX_STANDARD 20)

file(GLOB bittool_SRC "codec/*.c")
add_executable(bittool bittool.c ${bittool_SRC} )

set_property(TARGET bittool PROPERTY C_STANDARD 11)
target_compile_options(bittool PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(bittool PRIVATE fec)
