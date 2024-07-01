cmake_minimum_required(VERSION 3.14)

project(Temperature LANGUAGES CXX DESCRIPTION "Library for converting Fahrenheit to Celsius")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(Temperature STATIC
  temperature.cpp
  temperature.h
)

target_link_libraries(Temperature PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_compile_definitions(Temperature PRIVATE TEMPERATURE_LIBRARY)
target_include_directories(Temperature INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
