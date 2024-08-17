cmake_minimum_required(VERSION 3.16)
project(gui LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

set(SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/Gui.cpp")

add_library(gui STATIC ${SRC_FILES})

target_include_directories(gui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(gui PUBLIC sfml-graphics)
target_compile_features(gui PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET gui
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:gui>
        VERBATIM
    )
endif()

