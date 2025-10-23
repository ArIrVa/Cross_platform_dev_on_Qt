cmake_minimum_required(VERSION 3.14)

project(Searcher LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Concurrent )

set_source_files_properties(${SOURCE_FILES} PROPERTIES CHARACTER_SET UTF-8)

set(PROJECT_SOURCES
        main.cpp
        searcher.cpp
        searcher.h
        searcher.ui
        database.h
        database.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        searcher.ui
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
                                                    Qt${QT_VERSION_MAJOR}::Sql
                                                    Qt${QT_VERSION_MAJOR}::Concurrent
                                                    libcrypto-3-x64
                                                    libiconv-2
                                                    libintl-9
                                                    libpq
                                                    libssl-3-x64)

elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE   Qt${QT_VERSION_MAJOR}::Widgets
                                                    Qt${QT_VERSION_MAJOR}::Sql
                                                    Qt${QT_VERSION_MAJOR}::Concurrent )

else()
    message(FATAL_ERROR "Unknow OS")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets
                                      Qt6::Sql
                                      Qt6::Concurrent)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()



