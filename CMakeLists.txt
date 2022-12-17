# first build vst3sdk and paste root path here
set(vst3sdk_path "/Users/aidan/dev/vst/vst3_x86_64/vst3sdk") #e.g /Users/aidan/dev/vst3sdk
message("USING VST3SDK PATH PROVIDED: ${vst3sdk_path}")

cmake_minimum_required(VERSION 3.15)

set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE INTERNAL "" FORCE)

project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(imgui CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(portaudio CONFIG REQUIRED)
find_package(unofficial-concurrentqueue CONFIG REQUIRED)
find_package(rtmidi CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

# if(MSVC)
#     add_compile_definitions(SDL_MAIN_HANDLED)
#     add_compile_options(/W4 /WX)
# else()
#     add_compile_options(-Wall -Wextra -Wpedantic -Werror)
# endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

# Imnodes

add_library(imnodes)
target_sources(imnodes PRIVATE
    imnodes.h
    imnodes_internal.h
    imnodes.cpp)
target_include_directories(imnodes PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(imnodes PUBLIC imgui::imgui)

include_directories(${CMAKE_SOURCE_DIR} include vendor ${vst3sdk_path})
link_directories(${vst3sdk_path}/build/lib/)

file(GLOB SOURCES 
        src/*.cpp
        vendor/*.cpp
        vendor/EasyVst/EasyVst.cpp
        vendor/EasyVst/window.mm
        ${vst3sdk_path}/public.sdk/source/vst/hosting/connectionproxy.cpp
        ${vst3sdk_path}/public.sdk/source/vst/hosting/eventlist.cpp
        ${vst3sdk_path}/public.sdk/source/vst/hosting/hostclasses.cpp
        ${vst3sdk_path}/public.sdk/source/vst/hosting/module.cpp
        ${vst3sdk_path}/public.sdk/source/vst/hosting/parameterchanges.cpp
        ${vst3sdk_path}/public.sdk/source/vst/hosting/pluginterfacesupport.cpp
        ${vst3sdk_path}/public.sdk/source/vst/hosting/plugprovider.cpp
        ${vst3sdk_path}/public.sdk/source/vst/hosting/processdata.cpp
        ${vst3sdk_path}/public.sdk/source/vst/utility/stringconvert.cpp
        ${vst3sdk_path}/public.sdk/source/vst/vstinitiids.cpp
        ${vst3sdk_path}/public.sdk/source/vst/hosting/module_mac.mm
)

file(GLOB INCLUDES include/*.h vendor/*.h)
include_directories(vendor/EasyVst/EasyVst.h ${vst3sdk_path})

#message(STATUS "sources ${SOURCES}")
#message(STATUS "includes ${INCLUDES}")

add_executable(MyProject
    res/fonts/unifont-14.0.01.ttf
    ${SOURCES}
    ${INCLUDES}
    ${CMAKE_SOURCE_DIR}/src/main.cpp)

set(RESOURCE_FILES res/fonts/unifont-14.0.01.ttf)

# set_source_files_properties("/Users/aidan/dev/js/dummy/assets/icon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")


target_link_libraries(MyProject imnodes SDL2::SDL2 portaudio_static RtMidi::rtmidi unofficial::concurrentqueue::concurrentqueue magic_enum::magic_enum base pluginterfaces sdk_common sdk)

# set(png_path "/Users/aidan/dev/js/dummy/assets/icon.icns")
# target_sources(MyProject PUBLIC ${png_path})
# set_source_files_properties(${png_path} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

if (APPLE)
    target_link_libraries(MyProject "-framework OpenGL")
elseif(MSVC)
    target_link_libraries(MyProject "opengl32")
else()
    target_link_libraries(MyProject X11 Xext GL)
endif()

# set(RESOURCE_FILES /Users/aidan/dev/cpp/node_editor/res/fonts/unifont-14.0.01.ttf)

# for xcode generator 
if(CMAKE_GENERATOR STREQUAL Xcode)
    set(CMAKE_C_FLAGS "-x objective-c")
    set(CMAKE_CXX_FLAGS "-x objective-c++ -DRELEASE=1")
    set(CMAKE_EXE_LINKER_FLAGS "-framework Cocoa -framework AppKit -framework CoreData -framework Foundation")
    set_property (TARGET MyProject APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
    set_target_properties(MyProject PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_FRAMEWORK_IDENTIFIER org.cmake.MyProject
    RESOURCE "${RESOURCE_FILES}")
else() #for unix makefiles 
	set (CMAKE_CXX_FLAGS "-DRELEASE=1")
	set_property (TARGET MyProject APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc -ObjC++")
endif()

