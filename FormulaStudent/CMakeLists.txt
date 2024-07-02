cmake_minimum_required(VERSION 3.5)

project(FormulaStudent VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(Qt6 COMPONENTS Core Sql REQUIRED)
find_package(Qt6 COMPONENTS WebEngineWidgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS QuickWidgets)
find_package(Qt6 REQUIRED COMPONENTS Mqtt)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FormulaStudent
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        homePage.h
        homePage.cpp
        telemetryPage.h
        telemetryPage.cpp

        settingsPage.h settingsPage.cpp
        databaseManager.h databaseManager.cpp
        mapPage.cpp mapPage.h
        progressDialog.h progressDialog.cpp progressdialog.ui
        qcustomplot.h qcustomplot.cpp

    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET FormulaStudent APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(FormulaStudent SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(FormulaStudent
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Defines the path to the static libraries and the directory to include
set(TEMPERATURE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Utilities/Temperature")
target_include_directories(FormulaStudent PRIVATE ${TEMPERATURE_INCLUDE_DIR})
set(GEODISTANCE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Utilities/GeoDistance")
target_include_directories(FormulaStudent PRIVATE ${GEODISTANCE_INCLUDE_DIR})
set(NUMBERUTILS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Utilities/NumberUtils")
target_include_directories(FormulaStudent PRIVATE ${NUMBERUTILS_INCLUDE_DIR})
set(CANDATA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Processing/CANData")
target_include_directories(FormulaStudent PRIVATE ${CANDATA_INCLUDE_DIR})

# Set library paths based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TEMPERATURE_LIB_PATH "${CMAKE_SOURCE_DIR}/Utilities/build-Temperature-Desktop_Qt_6_6_1_MSVC2019_64bit-Debug/Temperature.lib")
    set(GEODISTANCE_LIB_PATH "${CMAKE_SOURCE_DIR}/Utilities/build-GeoDistance-Desktop_Qt_6_6_1_MSVC2019_64bit-Debug/GeoDistance.lib")
    set(NUMBERUTILS_LIB_PATH "${CMAKE_SOURCE_DIR}/Utilities/build-NumberUtils-Desktop_Qt_6_6_1_MSVC2019_64bit-Debug/NumberUtils.lib")
    set(CANDATA_LIB_PATH "${CMAKE_SOURCE_DIR}/Processing/build-CANData-Desktop_Qt_6_6_1_MSVC2019_64bit-Debug/CANData.lib")
else()
    set(TEMPERATURE_LIB_PATH "${CMAKE_SOURCE_DIR}/Utilities/build-Temperature-Desktop_Qt_6_6_1_MSVC2019_64bit-Release/Temperature.lib")
    set(GEODISTANCE_LIB_PATH "${CMAKE_SOURCE_DIR}/Utilities/build-GeoDistance-Desktop_Qt_6_6_1_MSVC2019_64bit-Release/GeoDistance.lib")
    set(NUMBERUTILS_LIB_PATH "${CMAKE_SOURCE_DIR}/Utilities/build-NumberUtils-Desktop_Qt_6_6_1_MSVC2019_64bit-Release/NumberUtils.lib")
    set(CANDATA_LIB_PATH "${CMAKE_SOURCE_DIR}/Processing/build-CANData-Desktop_Qt_6_6_1_MSVC2019_64bit-Release/CANData.lib")
endif()


target_link_libraries(FormulaStudent PRIVATE Qt6::Widgets)
target_link_libraries(FormulaStudent PRIVATE Qt6::Widgets Qt6::PrintSupport)
target_link_libraries(FormulaStudent PRIVATE Qt6::Core Qt6::Sql)
target_link_libraries(FormulaStudent PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(FormulaStudent PRIVATE Qt6::QuickWidgets)
target_link_libraries(FormulaStudent PRIVATE Qt6::Mqtt)

# Link to the precompiled custom library
target_link_libraries(FormulaStudent PRIVATE ${TEMPERATURE_LIB_PATH})
target_link_libraries(FormulaStudent PRIVATE ${GEODISTANCE_LIB_PATH})
target_link_libraries(FormulaStudent PRIVATE ${NUMBERUTILS_LIB_PATH})
target_link_libraries(FormulaStudent PRIVATE ${CANDATA_LIB_PATH})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FormulaStudent)
endif()
set_target_properties(FormulaStudent PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FormulaStudent
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FormulaStudent)
endif()


