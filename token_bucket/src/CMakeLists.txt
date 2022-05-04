cmake_minimum_required(VERSION 2.8.3)
project(tocken_bucket)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(TOKEN_BUCKET_SRCS
    main.cc
    token_timer.cc
)

add_executable(token_bucket ${TOKEN_BUCKET_SRCS})
target_link_libraries(token_bucket pthread)
