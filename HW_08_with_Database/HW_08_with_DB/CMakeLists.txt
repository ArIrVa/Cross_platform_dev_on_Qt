cmake_minimum_required(VERSION 3.5)

project(hw_08_with_DB VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Concurrent)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        database.h
        database.cpp
        dbdata.h
        dbdata.cpp
        dbdata.ui
)

add_executable(hw_08_with_DB
        ${PROJECT_SOURCES})



target_link_libraries(hw_08_with_DB PRIVATE Qt6::Widgets
                                               Qt6::Sql
                                               Qt6::Concurrent)



