cmake_minimum_required(VERSION 3.16)
project(SecureVaultNativeHost LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt modules required by the native host
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
)

qt_standard_project_setup()

# Native host sources only (NO core logic here)
file(GLOB_RECURSE PROJECT_SOURCES
    src/*.cpp
    src/*.h
    main.cpp
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link Qt + shared Core library
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Sql
        SecureVaultCore
)


# Copy Qt DLLs and plugins next to the executable so it runs outside Qt Creator / PATH
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Running windeployqt")
    endif()
endif()
