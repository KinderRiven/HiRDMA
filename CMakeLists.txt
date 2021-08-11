cmake_minimum_required(VERSION 2.8)

project(HIRDMA)

set (CMAKE_CXX_FLAGS "-O3 -std=c++11 -mrtm")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCCEH_STRINGKEY")

set(SRC_BASE ${PROJECT_SOURCE_DIR})

include_directories(
    ${SRC_BASE}/include
)

set(SRC
    ${SRC_BASE}/src/hi_rdma.cpp
)

set(LIBS
)

link_directories(
)

add_library(${PROJECT_NAME} STATIC ${SRC})
target_link_libraries(${PROJECT_NAME} ${LIBS})
