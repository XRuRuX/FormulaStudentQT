cmake_minimum_required(VERSION 3.14)

project(GeoDistance LANGUAGES CXX DESCRIPTION "Library for calculating the distance between two geographic coordinates")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

qt_add_library(GeoDistance STATIC
  geodistance.cpp
  geodistance.h
)

target_link_libraries(GeoDistance PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_compile_definitions(GeoDistance PRIVATE GEODISTANCE_LIBRARY)
target_include_directories(GeoDistance INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
