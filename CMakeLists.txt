cmake_minimum_required(VERSION 3.14)
project(cmake_qt)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_PREFIX_PATH "/home/suzhen/Qt5.13.0/5.13.0/gcc_64")

find_package(Qt5 COMPONENTS Gui Core Widgets REQUIRED)

find_package(CURL REQUIRED)

option(CURL_VERBOSE "enable curl log" OFF)

if (CURL_VERBOSE)
    add_definitions(-DCURL_VERBOSE)
endif ()


add_definitions(-D${CMAKE_BUILD_TYPE})

add_executable(cmake_qt
        forms/repo_form.ui
        forms/repos_form.cpp
        forms/repo_new_dialog.ui
        forms/repo_new_dialog.cpp
        forms/versions_form.ui
        forms/versions_form.cpp
        forms/versions_new_dialog.cpp
        forms/utils.cpp
        net/api.cpp
        net/http.cpp
        net/uploader.cpp
        resource.qrc
        main.cpp forms/lite_progress_dialog.cpp forms/lite_progress_dialog.h)

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