cmake_minimum_required(VERSION 3.13)


if(DEFINED ENV{VCPKG_HOME} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "tool chain file")
endif()

project(qvcs-gui)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


option(CURL_VERBOSE "enable curl log" ON)
set(HTTP_BASEURL "\"http://192.168.20.49:12136/api/v1\"" CACHE STRING "http base url for request")
set(Qt5_ROOT_DIR "/opt/Qt5.13.1/5.13.1/gcc_64" CACHE PATH "qt home dir")

list(APPEND CMAKE_PREFIX_PATH ${Qt5_ROOT_DIR})

find_package(Qt5 COMPONENTS Gui Core Widgets REQUIRED)

find_package(CURL REQUIRED)

if (CURL_VERBOSE)
    add_definitions(-DCURL_VERBOSE)
endif ()

add_definitions(-D${CMAKE_BUILD_TYPE} -DkHttpBaseURL=${HTTP_BASEURL})

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
endif (MSVC)

add_executable(qvcs-gui
        # ui
        forms/repo_form.ui
        forms/repo_new_dialog.ui
        forms/versions_form.ui
        forms/versions_new_dialog.ui
        # res
        resource.qrc
        # src
        forms/repos_form.cpp
        forms/repo_new_dialog.cpp
        forms/versions_form.cpp
        forms/versions_new_dialog.cpp
        forms/utils.cpp
        forms/lite_progress_dialog.cpp
        net/api.cpp
        net/http.cpp
        net/uploader.cpp
        main.cpp
        )

target_include_directories(qvcs-gui PUBLIC
        Qt5::Include
        ${CMAKE_SOURCE_DIR}
        ${CURL_INCLUDE_DIRS})

target_link_libraries(qvcs-gui
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        ${CURL_LIBRARIES})


#foreach(plugin ${Qt5Widgets_LIBRARIES})
#    get_target_property(_loc ${plugin} LOCATION)
#    message("Plugin ${plugin} is at location ${_loc}")
#endforeach()
