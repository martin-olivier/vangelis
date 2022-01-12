cmake_minimum_required(VERSION 3.17)

project(IO_Tester)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

option(UNIT_TESTS "When sets to ON, build the unit tests" OFF)

if(UNIT_TESTS)
    set(BIN unit_tests)
    include(import/FetchGoogleTest.cmake)
else()
    set(BIN IO_Tester)
endif()

if(UNIX)
    add_compile_options(-Wall -Wextra -Weffc++)
elseif(WIN32)
    add_compile_options(/W4)
endif()

set(MAIN src/main.cpp)

set(SOURCES
    src/IO_Tester.cpp
    src/Utils.cpp
    src/Parsing.cpp
    src/Test.cpp
    src/Diff.cpp
    src/Updater.cpp
)

set(TEST_SOURCES
    tests/src/parser.cpp
)

if(UNIT_TESTS)
    add_executable(${BIN}
        ${SOURCES}
        ${TEST_SOURCES}
    )
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(${BIN})
    target_link_libraries(${BIN} PRIVATE gtest_main)
else()
    add_executable(${BIN}
        ${MAIN}
        ${SOURCES}
    )
endif()

target_link_libraries(${BIN} PRIVATE pthread)

target_include_directories(${BIN} PRIVATE include)