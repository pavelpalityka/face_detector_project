cmake_minimum_required(VERSION 3.16)
project(FacesProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(MODELS_SRC_DIR ${CMAKE_SOURCE_DIR}/faceslib/models)

# copy model in bin
file(COPY ${MODELS_SRC_DIR} DESTINATION ${BIN_DIR})

# ---- PACKAGES ----
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

# JSONCPP detection
if(WIN32)
    find_package(JsonCpp REQUIRED)
    set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIRS})
    set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARIES})
elseif(APPLE)
    # On macOS, first try find_package (brew), fallback to default Homebrew paths
    find_package(JsonCpp)
    if(NOT JSONCPP_FOUND)
        message(STATUS "JsonCpp not found via find_package, trying Homebrew paths...")
        find_path(JSONCPP_INCLUDE_DIR json/json.h
            PATHS /opt/homebrew/include /usr/local/include
            NO_DEFAULT_PATH)
        find_library(JSONCPP_LIBRARY jsoncpp
            PATHS /opt/homebrew/lib /usr/local/lib
            NO_DEFAULT_PATH)
    endif()
elseif(UNIX)
    # On Linux, use find_package first, can add custom paths if needed
    find_package(JsonCpp REQUIRED)
endif()

# Verify that paths were found
if(NOT JSONCPP_INCLUDE_DIR OR NOT JSONCPP_LIBRARY)
    message(FATAL_ERROR "JsonCpp not found. Set JSONCPP_INCLUDE_DIR and JSONCPP_LIBRARY manually.")
else()
    message(STATUS "JsonCpp include: ${JSONCPP_INCLUDE_DIR}")
    message(STATUS "JsonCpp library: ${JSONCPP_LIBRARY}")
endif()

# ---- SUBDIRECTORIES ----
add_subdirectory(faceslib)
add_subdirectory(facesapp)
add_subdirectory(tests)
