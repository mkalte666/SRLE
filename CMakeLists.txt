cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 11)
project(SRLE)

add_executable(runtests
    src/tests.cpp 
    include/srle.h
    )

add_executable(srledecoder
    src/decoder.cpp 
    include/srle.h
    )
    
add_executable(srleencoder
    src/encoder.cpp 
    include/srle.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()
add_test(NAME allTests COMMAND runtests)