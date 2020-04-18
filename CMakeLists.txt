cmake_minimum_required(VERSION 3.10)
project(dotmatrix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:\\Qt\\5.12.6\\mingw73_32\\")
set(CMAKE_PREFIX_PATH "/Users/pscott/Qt/5.12.6/clang_64")

find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_add_resources(resources.qrc)

add_executable(dotmatrix cpu.cpp ppu.cpp gbthread.cpp gbwidget.cpp main.cpp mmu.cpp mmu.h)
target_link_libraries(dotmatrix Qt5::Widgets)