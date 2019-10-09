cmake_minimum_required(VERSION 3.14)
project(cmake_qt)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


LIST(APPEND CMAKE_PREFIX_PATH "/home/suzhen/Qt5.13.0/5.13.0/gcc_64")

find_package(Qt5 COMPONENTS Gui Core Widgets REQUIRED)

find_package(CURL REQUIRED)


add_executable(cmake_qt
        forms/main_window.ui
        forms/main_window.cpp
        forms/new_repo_dialog.ui
        forms/new_repo_dialog.cpp
        net/api.cpp
        net/api.h
        net/http.cpp
        net/http.h
        resource.qrc
        main.cpp)


target_include_directories(cmake_qt PUBLIC
        Qt5::Include
        ${CMAKE_SOURCE_DIR}
        ${CURL_INCLUDE_DIRS})

target_link_libraries(cmake_qt
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        ${CURL_LIBRARIES})

#foreach(plugin ${Qt5Widgets_LIBRARIES})
#    get_target_property(_loc ${plugin} LOCATION)
#    message("Plugin ${plugin} is at location ${_loc}")
#endforeach()