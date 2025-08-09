cmake_minimum_required(VERSION 3.16)
project(QtButtonGridApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(QtButtonGridApp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    CustomTextEdit.h

)

target_link_libraries(QtButtonGridApp PRIVATE Qt6::Widgets) 
