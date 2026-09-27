cmake_minimum_required(VERSION 3.20)
project(BinaryArchiver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#  Core Archiver Library
add_library(archiver_core
    src/Archiver.cpp
)
target_include_directories(archiver_core PUBLIC include)

#  Main CLI Tool
add_executable(arc src/main.cpp)
target_link_libraries(arc PRIVATE archiver_core)

#  Unit Testing Setup
include(CTest)
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

#  Test Executable
add_executable(arc_tests tests/test_main.cpp)
target_link_libraries(arc_tests PRIVATE archiver_core Catch2::Catch2WithMain)

add_test(NAME AllTests COMMAND arc_tests)


include(GNUInstallDirs)

install(TARGETS arc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)