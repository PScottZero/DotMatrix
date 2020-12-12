cmake_minimum_required(VERSION 3.10)
project(dotmatrix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:\\Qt\\5.15.2\\mingw81_64\\lib\\cmake")
set(CMAKE_PREFIX_PATH "/Users/paulscott/Qt/5.15.2/clang_64")

find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_add_resources(resource.qrc)

add_executable(dotmatrix cpu.cpp ppu.cpp gbThread.cpp displayWidget.cpp main.cpp mmu.cpp mmu.h dmWindow.cpp dmWindow.h resource.qrc palette.cpp palette.h)
target_link_libraries(dotmatrix Qt5::Widgets)