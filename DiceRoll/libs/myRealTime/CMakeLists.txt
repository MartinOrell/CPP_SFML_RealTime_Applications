cmake_minimum_required(VERSION 3.13)

project(myRealTime)
set(CMAKE_CXX_STANDARD 17)

set(SRC_FILES src/CapsuleRunner.cpp src/MessageHandler.cpp src/MessageManager.cpp src/SendMessage.cpp src/Timer.cpp)

add_library(myRealTime STATIC ${SRC_FILES})

target_include_directories(myRealTime PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories(myRealTime PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/mod_include/")