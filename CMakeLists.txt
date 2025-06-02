cmake_minimum_required(VERSION 3.10)
project(MaterialViewer LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/glad.c
)

add_executable(materialviewer ${SOURCES})

target_include_directories(materialviewer
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/freetype/include"
)

if (WIN32)
    target_include_directories(materialviewer PRIVATE
        "C:/msys64/ucrt64/include"
    )
    target_link_directories(materialviewer PRIVATE
        "${CMAKE_SOURCE_DIR}/lib"
        "C:/msys64/ucrt64/lib"
    )

    target_compile_options(materialviewer PRIVATE
        -g
    )

    target_link_libraries(materialviewer
        PRIVATE
            zip
            zstd
            freetype
            glfw3dll
            comdlg32
    )
endif()

if (UNIX AND NOT APPLE)
    target_compile_options(materialviewer PRIVATE
        -g
    )

    find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Core)
    target_compile_definitions(materialviewer PRIVATE
        QT_WIDGETS_LIB
        QT_GUI_LIB
        QT_CORE_LIB
    )
    target_include_directories(materialviewer PRIVATE
        /usr/include/qt/QtWidgets
        /usr/include/qt
        /usr/include/qt/QtGui
        /usr/include/qt/QtCore
    )

    find_package(Freetype REQUIRED)

    find_package(glfw3 3.3 QUIET)
    if (glfw3_FOUND)
        set(GLFW_LIB_TARGET glfw)
    else()
        set(GLFW_LIB_TARGET glfw)
    endif()

    find_library(ZIP_LIB   zip)
    find_library(ZSTD_LIB  zstd)

    target_link_libraries(materialviewer
        PRIVATE
            ${ZIP_LIB}
            ${ZSTD_LIB}
            Freetype::Freetype
            Qt5::Widgets
            Qt5::Gui
            Qt5::Core
            ${GLFW_LIB_TARGET}
            dl
            m
    )
endif()