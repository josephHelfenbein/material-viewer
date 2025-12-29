cmake_minimum_required(VERSION 3.10)
project(MaterialViewer LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/glad.c
    ${CMAKE_SOURCE_DIR}/src/embedded_resources.cpp
)

if (WIN32)
    set(SOURCES ${SOURCES} ${CMAKE_SOURCE_DIR}/src/resources/materialviewer.rc)
endif()

add_executable(materialviewer ${SOURCES})

target_include_directories(materialviewer
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/freetype/include"
        "${CMAKE_SOURCE_DIR}/src"
)

if (WIN32)
    target_include_directories(materialviewer PRIVATE
        "C:/msys64/ucrt64/include"
    )
    target_compile_definitions(materialviewer PRIVATE
        ZIP_STATIC
    )
    set(MSYS2_LIB "C:/msys64/ucrt64/lib")
    
    target_link_directories(materialviewer PRIVATE
        "${CMAKE_SOURCE_DIR}/lib"
        "${MSYS2_LIB}"
    )
    target_compile_options(materialviewer PRIVATE -g)
    target_link_options(materialviewer PRIVATE 
        -static-libgcc 
        -static-libstdc++
        "-Wl,--whole-archive" "${MSYS2_LIB}/libwinpthread.a" "-Wl,--no-whole-archive"
    )
    
    target_link_libraries(materialviewer
        PRIVATE
            "${MSYS2_LIB}/libglfw3.a"
            "${MSYS2_LIB}/libfreetype.a"
            "${MSYS2_LIB}/libzip.a"
            "${MSYS2_LIB}/libzstd.a"
            "${MSYS2_LIB}/libbz2.a"
            "${MSYS2_LIB}/libbrotlidec.a"
            "${MSYS2_LIB}/libbrotlicommon.a"
            "${MSYS2_LIB}/libharfbuzz.a"
            "${MSYS2_LIB}/libgraphite2.a"
            "${MSYS2_LIB}/libpng.a"
            "${MSYS2_LIB}/libz.a"
            "${MSYS2_LIB}/liblzma.a"
            "${MSYS2_LIB}/libglib-2.0.a"
            "${MSYS2_LIB}/libintl.a"
            "${MSYS2_LIB}/libiconv.a"
            "${MSYS2_LIB}/libpcre2-8.a"
            opengl32
            gdi32
            user32
            kernel32
            comdlg32
            shell32
            winmm
            rpcrt4
            ole32
            shlwapi
            ws2_32
            bcrypt
    )
endif()

if (UNIX AND NOT APPLE)

    target_compile_options(materialviewer PRIVATE -g)

    find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Core)

    target_compile_definitions(materialviewer PRIVATE
        QT_WIDGETS_LIB
        QT_GUI_LIB
        QT_CORE_LIB
    )

    find_package(Freetype REQUIRED)

    find_package(glfw3 3.3 REQUIRED)

    find_library(ZSTD_LIB zstd REQUIRED)
    find_library(ZIP_LIB zip REQUIRED)

    target_link_libraries(materialviewer
        PRIVATE
            Qt5::Widgets
            Qt5::Gui
            Qt5::Core
            Freetype::Freetype
            glfw
            ${ZSTD_LIB}
            ${ZIP_LIB}
            dl
            m
    )

endif()

if (APPLE)
    target_compile_options(materialviewer PRIVATE -g)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")
    find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Core)
    target_compile_definitions(materialviewer PRIVATE
        QT_WIDGETS_LIB
        QT_GUI_LIB
        QT_CORE_LIB
        GL_SILENCE_DEPRECATION
    )

    find_package(Freetype REQUIRED)
    find_package(glfw3 3.3 REQUIRED)
    find_library(ZSTD_LIB zstd REQUIRED)
    find_library(ZIP_LIB zip REQUIRED)

    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREVIDEO_FRAMEWORK CoreVideo REQUIRED)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)

    target_link_libraries(materialviewer
        PRIVATE
            Qt5::Widgets
            Qt5::Gui
            Qt5::Core
            Freetype::Freetype
            glfw
            ${ZSTD_LIB}
            ${ZIP_LIB}
            ${COCOA_FRAMEWORK}
            ${IOKIT_FRAMEWORK}
            ${COREVIDEO_FRAMEWORK}
            ${OPENGL_FRAMEWORK}
    )

endif()