cmake_minimum_required(VERSION 3.14)

project(NumberUtils LANGUAGES CXX DESCRIPTION "Library for calculating, manipulating and converting numbers")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

qt_add_library(NumberUtils STATIC
  numberutils.cpp
  numberutils.h
)

target_link_libraries(NumberUtils PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_compile_definitions(NumberUtils PRIVATE NUMBERUTILS_LIBRARY)
target_include_directories(NumberUtils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
