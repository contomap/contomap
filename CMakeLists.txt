cmake_minimum_required(VERSION 3.25..3.27.4)
project(contomap)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable C++20 support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
cmake_policy(SET CMP0135 NEW)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif ()
endif ()
#set(raylib_VERBOSE 1)

add_library(raygui STATIC "${PROJECT_SOURCE_DIR}/vendor/raygui/src/raygui.c")
target_include_directories(raygui
        PRIVATE
        "${PROJECT_SOURCE_DIR}/vendor/raygui/include/raygui"
        PUBLIC
        "${PROJECT_SOURCE_DIR}/vendor/raygui/include"
)
target_link_libraries(raygui PUBLIC raylib)
target_compile_definitions(raygui PRIVATE RAYGUI_IMPLEMENTATION)


# This project

file(GLOB_RECURSE LIB_EDITOR_SOURCES "${PROJECT_SOURCE_DIR}/editor/src/cpp/*.cpp")
add_library(contomap-editor STATIC ${LIB_EDITOR_SOURCES})
target_include_directories(contomap-editor PUBLIC "${PROJECT_SOURCE_DIR}/editor/src/h")

file(GLOB_RECURSE LIB_EDITOR_TEST_SOURCES "${PROJECT_SOURCE_DIR}/editor/test/*.cpp")
add_executable(contomap-editor-test ${LIB_EDITOR_TEST_SOURCES})
target_link_libraries(contomap-editor-test
        PUBLIC
        contomap-editor
        GTest::gmock_main
)


file(GLOB_RECURSE LIB_FRONTEND_SOURCES "${PROJECT_SOURCE_DIR}/frontend/src/cpp/*.cpp")
add_library(contomap-frontend STATIC ${LIB_FRONTEND_SOURCES})
target_include_directories(contomap-frontend PUBLIC "${PROJECT_SOURCE_DIR}/frontend/src/h")
target_link_libraries(contomap-frontend
        PUBLIC
        contomap-editor
        raygui
        raylib
)

file(GLOB_RECURSE LIB_FRONTEND_TEST_SOURCES "${PROJECT_SOURCE_DIR}/frontend/test/*.cpp")
add_executable(contomap-frontend-test ${LIB_FRONTEND_TEST_SOURCES})
target_link_libraries(contomap-frontend-test
        PUBLIC
        contomap-frontend
        GTest::gmock_main
)

file(GLOB_RECURSE LIB_APPLICATION_SOURCES "${PROJECT_SOURCE_DIR}/application/src/cpp/*.cpp")
add_library(contomap-application STATIC ${LIB_APPLICATION_SOURCES})
target_include_directories(contomap-application PUBLIC "${PROJECT_SOURCE_DIR}/application/src/h")
target_link_libraries(contomap-application
        PUBLIC
        contomap-editor
        contomap-frontend
)

# TODO: options for all contomap targets
target_compile_options(contomap-frontend PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
        $<$<CXX_COMPILER_ID:MSVC>:-W4>
)

if (${PLATFORM} STREQUAL "Desktop")
    add_executable(contomap "${PROJECT_SOURCE_DIR}/main-desktop/main.cpp")
    target_link_libraries(contomap
            contomap-application
            raylib
    )
    if (APPLE)
        # Checks if OSX and links appropriate frameworks (Only required on MacOS)
        target_link_libraries(contomap "-framework IOKit")
        target_link_libraries(contomap "-framework Cocoa")
        target_link_libraries(contomap "-framework OpenGL")
    elseif (WIN32)
        # In RelWithDebInfo builds, specify windows subsystem in order to avoid opening a console window.
        set(SUBSYSTEM_LINKER_OPTIONS "-Wl,-subsystem,windows")
        target_link_options(contomap PRIVATE $<$<CONFIG:RelWithDebInfo>:${SUBSYSTEM_LINKER_OPTIONS}>)
    endif ()
elseif (${PLATFORM} STREQUAL "Web")
    add_executable(contomap-wasm "${PROJECT_SOURCE_DIR}/main-wasm/main.cpp")
    target_link_libraries(contomap-wasm
            contomap-application
            raylib
    )
    # Tell Emscripten to build an example.html file.
    set_target_properties(contomap-wasm PROPERTIES SUFFIX ".html")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
endif ()
