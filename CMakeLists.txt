cmake_minimum_required(VERSION 3.5)
project(AltCraft)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
endif()

##########
# SOURCES
##########

include(PatchSrcList)

file(GLOB MANIFEST "./src/*.manifest")
file(GLOB SOURCES "./src/*.cpp")
file(GLOB HEADERS "./src/*.hpp")

if(ANDROID)
    PatchSrc("./src/platform/Linux")
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/src/platform/${CMAKE_SYSTEM_NAME}")
    PatchSrc("./src/platform/${CMAKE_SYSTEM_NAME}")
else()
    PatchSrc("./src/platform/SDL")
endif()

add_executable(AltCraft ${HEADERS} ${SOURCES} ${MANIFEST})


################
# CONFIGURATION
################

option(ENABLE_LOCAL_OPTIMIZATIONS "Enable local compiler optimizations? (e.g. -mtune=native)" OFF)
option(ENABLE_PCH "Enable precompiled header" OFF)
option(ENABLE_OPTICK "Enable profiler \"Optick \"?" OFF)

if(ENABLE_PCH)
	target_precompile_headers(AltCraft PUBLIC "./src/AltCraft.h")
endif()

#Configure Optick
if (ENABLE_OPTICK)
	add_definitions(-DUSE_OPTICK=1)
else()
	add_definitions(-DUSE_OPTICK=0)
endif()

#Configure easlylogging++
set(LOGGER_DEFINITIONS "-DELPP_THREAD_SAFE -DELPP_FEATURE_PERFORMANCE_TRACKING")

#Set compiler's flags and setup platfrom-dependent libraries
if(CMAKE_COMPILER_IS_GNUCXX)
    set(LOGGER_DENITIONS "${LOGGER_DEFINITIPNS} -DELPP_FEATURE_CRASH_LOG")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fno-plt -Wall -pedantic -Wextra -Wno-error=unused-parameter -fdata-sections -ffunction-sections -Wl,--gc-sections -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -p")
    set(PLATFORM_LIBRARIES "stdc++fs")

    if(ENABLE_LOCAL_OPTIMIZATIONS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native")
    endif()
endif()

if(MSVC)
    set(PLATFORM_LIBRARIES "ws2_32")
    add_definitions("-D_USE_MATH_DEFINES -DNOMINMAX")

    if(CMAKE_BUILD_TYPE MATCHES Release)
        target_compile_definitions(AltCraft PRIVATE USE_SDL_MAIN)
        set_target_properties(AltCraft PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    endif()
endif(MSVC)

add_definitions(${LOGGER_DEFINITIONS})

set(AUDIO_LIBRARY "OpenAL" CACHE STRING "Audio API")
set_property(CACHE AUDIO_LIBRARY PROPERTY STRINGS "OpenAL")

target_link_libraries(AltCraft ${PLATFORM_LIBRARIES})

############
# LIBRARIES
############

#Setup dependencies (GLEW, glm, nlohmann/json, easylogging++)
add_subdirectory(external)
target_include_directories(AltCraft PUBLIC external/include)
target_link_libraries(AltCraft deps nlohmann_json sol2)

target_include_directories(AltCraft PUBLIC external/optick)
if (${ENABLE_OPTICK})
	target_link_libraries(AltCraft optick)
endif()

#Setup SDL2
find_package(SDL2 REQUIRED)
target_link_libraries(AltCraft ${SDL2_LIBRARY})
target_include_directories(AltCraft PUBLIC ${SDL2_INCLUDE_DIR})

if(NOT (ANDROID OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
#Setup SDL2_net
find_package(SDL2_net REQUIRED)
target_link_libraries(AltCraft ${SDL2_NET_LIBRARIES})
target_include_directories(AltCraft PUBLIC ${SDL2_NET_INCLUDE_DIRS})
endif()

#Setup OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(AltCraft ${OPENGL_LIBRARIES})
target_include_directories(AltCraft PUBLIC ${OPENGL_INCLUDE_DIRS})

#Setup Zlib
find_package(ZLIB REQUIRED)
target_link_libraries(AltCraft ${ZLIB_LIBRARIES})
target_include_directories(AltCraft PUBLIC ${ZLIB_INCLUDE_DIRS})

#Setup Lua
find_package(Lua REQUIRED)
target_link_libraries(AltCraft ${LUA_LIBRARIES})
target_include_directories(AltCraft PUBLIC ${LUA_INCLUDE_DIR})

if(AUDIO_LIBRARY STREQUAL OpenAL)
    #Setup OpenAL
    target_sources(AltCraft PRIVATE "./src/audio/OpenAL.cpp")
    add_definitions(-DAC_AUDIO_OPENAL)
    find_package(OpenAL REQUIRED)
    target_link_libraries(AltCraft ${OPENAL_LIBRARY})
    target_include_directories(AltCraft PUBLIC ${OPENAL_INCLUDE_DIR})
endif()

#################
# COPY RESOURCES
#################

add_custom_command(TARGET AltCraft PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/cwd $<TARGET_FILE_DIR:AltCraft>)
