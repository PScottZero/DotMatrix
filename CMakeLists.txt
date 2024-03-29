cmake_minimum_required(VERSION 3.5)

project(DotMatrix VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/icon.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/emulator/cgb.cpp
        src/emulator/cgb.h
        src/emulator/cpu.cpp
        src/emulator/cpu.h
        src/emulator/memory.cpp
        src/emulator/memory.h
        src/emulator/ppu.cpp
        src/emulator/ppu.h
        src/emulator/timers.cpp
        src/emulator/timers.h
        src/emulator/controls.cpp
        src/emulator/controls.h
        src/emulator/bootstrap.cpp
        src/emulator/bootstrap.h
        src/emulator/log.cpp
        src/emulator/log.h
        src/emulator/types.h
        src/emulator/mbc.cpp
        src/emulator/mbc.h
        src/emulator/rtc.cpp
        src/emulator/rtc.h
        src/emulator/apu.cpp
        src/emulator/apu.h
        src/ui/mainwindow.cpp
        src/ui/mainwindow.h
        src/ui/mainwindow.ui
        src/ui/keybindingswindow.cpp
        src/ui/keybindingswindow.h
        src/ui/keybindingswindow.ui
        src/ui/vramviewer.cpp
        src/ui/vramviewer.h
        src/ui/vramviewer.ui
        src/ui/palettes.cpp
        src/ui/palettes.h
        src/ui/settings.cpp
        src/ui/settings.h
)

qt_add_resources(PROJECT_SOURCES resource.qrc)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DotMatrix
        MANUAL_FINALIZATION
        MACOSX_BUNDLE
        ${PROJECT_SOURCES}
        ${app_icon_macos}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DotMatrix APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DotMatrix SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DotMatrix
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DotMatrix PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(DotMatrix PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS DotMatrix
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DotMatrix)
endif()
