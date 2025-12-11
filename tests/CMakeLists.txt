cmake_minimum_required(VERSION 3.16)

project(FacesTests LANGUAGES CXX)

# ---- GoogleTest FetchContent ----
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
enable_testing()

file(GLOB TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(tests ${TEST_SOURCES})

set_target_properties(tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

target_include_directories(tests PRIVATE
    ${JSONCPP_INCLUDE_DIR}
)

target_link_libraries(tests
    PRIVATE
        faceslib
        GTest::gtest
        GTest::gtest_main
        ${JSONCPP_LIBRARY}
)

target_compile_features(tests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(tests DISCOVERY_MODE PRE_TEST)
