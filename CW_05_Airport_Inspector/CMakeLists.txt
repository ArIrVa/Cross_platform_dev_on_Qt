cmake_minimum_required(VERSION 3.5)

project(CW_05_Airport_Inspector VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Concurrent Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Concurrent Charts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        database.h
        database.cpp
        graphic.h
        graphic.cpp
        graphic.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

if(WIN32)
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/PG_Libs)
    target_link_libraries(${PROJECT_NAME} PRIVATE   Qt${QT_VERSION_MAJOR}::Widgets
                                                    Qt${QT_VERSION_MAJOR}::Concurrent
                                                    Qt${QT_VERSION_MAJOR}::Sql
                                                    Qt${QT_VERSION_MAJOR}::Charts
                                                    libcrypto-3-x64
                                                    libiconv-2
                                                    libintl-9
                                                    libpq
                                                    libssl-3-x64)

elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE   Qt${QT_VERSION_MAJOR}::Widgets
                                                    Qt${QT_VERSION_MAJOR}::Concurrent
                                                    Qt${QT_VERSION_MAJOR}::Sql
                                                    Qt${QT_VERSION_MAJOR}::Charts)
else()
    message(FATAL_ERROR "Unknow OS")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "airinspect"
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(CW_05_Airport_Inspector PRIVATE Qt6::Widgets
                                               Qt6::Sql
                                               Qt6::Concurrent
                                               Qt6::Charts)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

