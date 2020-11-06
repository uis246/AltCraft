cmake_minimum_required(VERSION 3.5)

project(deps)

file(GLOB SOURCES "./src/*")
file(GLOB_RECURSE HEADERS "./include/*")

include_directories(./include/)

add_library(deps STATIC ${SOURCES} ${HEADERS})

if (ENABLE_OPTICK)
    file(GLOB OPTICK_SRC "./optick/*.cpp")
    add_library(optick STATIC ${OPTICK_SRC})
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann)
add_subdirectory(sol2)
