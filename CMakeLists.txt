cmake_minimum_required(VERSION 3.5)

project(GLFW-CMake-starter)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Building for 64-bit")
else()
    message(STATUS "Building for 32-bit")
endif()

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")
set(GLAD_GL "")

add_subdirectory(glfw)

option(GLFW-CMAKE-STARTER-USE-GLFW-GLAD "Use GLAD from GLFW" ON)

if (GLFW-CMAKE-STARTER-USE-GLFW-GLAD)
    include_directories("${GLFW_SOURCE_DIR}/deps" "/irrklang/include")
    set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h")
endif()

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

set(GLFW-CMAKE-STARTER-SRC
        main.cpp
        objects/boxes.h
        objects/chemsynth.cpp
        objects/boxes.cpp
        objects/chemsynth.h
        stb_image/stb_image.h
        stb_image/stb_image.cpp
        objects/textures.cpp
        objects/textures.h
        objects/sound.cpp
        objects/sound.h
)

add_executable(GLFW-CMake-starter ${GLFW-CMAKE-STARTER-SRC} ${GLAD_GL})
target_link_libraries(GLFW-CMake-starter ${OPENGL_LIBRARIES} glfw ${GLEW_LIB} winmm)

if (MSVC)
    if (${CMAKE_VERSION} VERSION_LESS "3.6.0")
        message("\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n")
    else()
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GLFW-CMake-starter)
    endif()
endif()