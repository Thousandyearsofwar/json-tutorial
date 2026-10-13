cmake_minimum_required (VERSION 3.10)
project (leptjson_test C)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

add_library(leptjson leptjson.c)
add_executable(leptjson_test test.c)
target_link_libraries(leptjson_test leptjson)

add_library(IeJson IeJson/Iejson.c)
add_executable(IejsonTest IeJson/IejsonTest.c)
target_link_libraries(IejsonTest IeJson)