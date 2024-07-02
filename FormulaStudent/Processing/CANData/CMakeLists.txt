cmake_minimum_required(VERSION 3.14)

project(CANData LANGUAGES CXX DESCRIPTION "Library for decoding and processing CAN Data")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

qt_add_library(CANData STATIC
  candata.cpp
  candata.h
)

# Defines the path to the static libraries and the directory to include
set(TEMPERATURE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../../Utilities/Temperature")
target_include_directories(CANData PRIVATE ${TEMPERATURE_INCLUDE_DIR})
set(NUMBERUTILS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../../Utilities/NumberUtils")
target_include_directories(CANData PRIVATE ${NUMBERUTILS_INCLUDE_DIR})

# Set library paths based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TEMPERATURE_LIB_PATH "${CMAKE_SOURCE_DIR}/../../Utilities/build-Temperature-Desktop_Qt_6_6_1_MSVC2019_64bit-Debug/Temperature.lib")
    set(NUMBERUTILS_LIB_PATH "${CMAKE_SOURCE_DIR}/../../Utilities/build-NumberUtils-Desktop_Qt_6_6_1_MSVC2019_64bit-Debug/NumberUtils.lib")
else()
    set(TEMPERATURE_LIB_PATH "${CMAKE_SOURCE_DIR}/../../Utilities/build-Temperature-Desktop_Qt_6_6_1_MSVC2019_64bit-Release/Temperature.lib")
    set(NUMBERUTILS_LIB_PATH "${CMAKE_SOURCE_DIR}/../../Utilities/build-NumberUtils-Desktop_Qt_6_6_1_MSVC2019_64bit-Release/NumberUtils.lib")
endif()

# Link to the precompiled custom library
target_link_libraries(CANData PRIVATE ${TEMPERATURE_LIB_PATH})
target_link_libraries(CANData PRIVATE ${NUMBERUTILS_LIB_PATH})

target_link_libraries(CANData PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_compile_definitions(CANData PRIVATE CANDATA_LIBRARY)
target_include_directories(CANData INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
