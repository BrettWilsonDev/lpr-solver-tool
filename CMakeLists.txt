#cmake starter

cmake_minimum_required(VERSION 3.22)
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)

##########################################################################################
# Add dependencies with FetchContent
##########################################################################################

set(LIBRARIES ${PROJECT_NAME})

include(FetchContent)

function(add_git_dependency libName gitURL gitTag buildEnabled)
    if(buildEnabled)
        FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG        ${gitTag}
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
        )
        FetchContent_MakeAvailable(${libName})

        # exclude lib to avoid errors
        if(NOT "${libName}" STREQUAL "libname" AND NOT "${libName}" STREQUAL "rlImGui")
            # target_compile_options(${libName} PRIVATE "-w")
        endif()
    endif()
endfunction()

#add new dependencies here

# toggle libs to be used with ON and OFF
set(useRaylib ON)
if(useRaylib)
    set(dep raylib)
    add_git_dependency(
        ${dep}
        https://github.com/raysan5/raylib.git
        head
        ${useRaylib}
    )

    # FetchContent magic
    FetchContent_GetProperties(${dep})
    if(NOT raylib_POPULATED)
        FetchContent_Populate(${dep})

        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
        set(BUILD_TESTING OFF)
        set(CUSTOMIZE_BUILD ON) 
        
        add_subdirectory(${${dep}_SOURCE_DIR} ${${dep}_BINARY_DIR})
    endif()

    # add the dependency here to be linked
    list(APPEND LIBRARIES ${dep})
endif()

set(useRlImGui ON)
if(useRlImGui)
    add_git_dependency(
        RlImGui
        https://github.com/raylib-extras/rlImGui.git
        head
        ${useRlImGui}
    )

    file(REMOVE_RECURSE build/_deps/rlimgui-src/examples)

    file(GLOB files_to_copy "${CMAKE_SOURCE_DIR}/build/_deps/rlimgui-src/*")
    file(COPY ${files_to_copy} DESTINATION ${CMAKE_SOURCE_DIR}/src/imgui)
endif()

set(useImGui ON)
if(useImGui)
    add_git_dependency(
        imGui
        https://github.com/ocornut/imgui.git
        head
        ${useImGui}
    )

    file(REMOVE_RECURSE build/_deps/imgui-src/examples)
    file(REMOVE_RECURSE build/_deps/imgui-src/misc)
    file(REMOVE_RECURSE build/_deps/imgui-src/backends)
    file(REMOVE_RECURSE build/_deps/imgui-src/.editorconfig)
    file(REMOVE_RECURSE build/_deps/imgui-src/.github)
    
    file(GLOB files_to_copy "${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/*")
    file(COPY ${files_to_copy} DESTINATION ${CMAKE_SOURCE_DIR}/src/imgui)
endif()

##########################################################################################
# Project executable setup
##########################################################################################

# Adding our source files
# Define PROJECT_SOURCES as a list of all source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")

# Define PROJECT_INCLUDE to be the path to the include directory of the project
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/")

# Declaring our executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})

#  dont forget to add libs here
# target_link_libraries(${PROJECT_NAME} PRIVATE libname)
target_link_libraries(${LIBRARIES})

make_directory(../src/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/imgui)

##########################################################################################
# custom commands
##########################################################################################

if(ASYNCIFY)
  # Add Raylib's async support
  add_definitions(-DRAYLIB_ASYNC_ENABLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASYNCIFY")

  # Add the -O3 optimization flag
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

##########################################################################################
# Project build settings
##########################################################################################

# Setting ASSETS_PATH
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    message("has console window")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -DDEBUG")
else()
    if(WIN32)
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
      add_compile_definitions(NDEBUG)
    endif()
endif()


# Set common compiler flags
# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wswitch")
# Set common compiler flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if (NOT DEFINED PLATFORM)
    set(PLATFORM "default")
endif()

if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an .html file.
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Os")
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -Os -Wall -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --preload-file assets/ --shell-file ../src/minshell.html")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -Os -Wall -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --shell-file ../src/minshell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    # target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="/assets/") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
endif()

# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()
