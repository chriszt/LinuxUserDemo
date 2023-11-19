cmake_minimum_required(VERSION 3.5.0)
project(SdJournalDemo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0")

set(SRC_LIST main.cpp SdJournalTest.h SdJournalTest.cpp Utils.h Utils.cpp Parser.h Parser.cpp RemoteCache.h RemoteCache.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} systemd pthread)
