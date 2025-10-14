cmake_minimum_required(VERSION 3.16)
project(beatplate)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/SFML-3.0.0/lib")

add_definitions(-DSFML_STATIC)

# diretorios que vão ser fonte de arquivos que serão incluidos
include_directories(
    ${CMAKE_SOURCE_DIR}
    utils
    shaders
    libs
    libs/SFML-3.0.0/include
    game_states
    game_logic
)

file(GLOB_RECURSE CORE_SOURCES
    "main.cpp"
    "state_stack.cpp"
)

file(GLOB_RECURSE UTILITIES_SOURCES
    "utils/*.cpp"
)

file(GLOB_RECURSE GAME_LOGIC_SOURCES
    "game_logic/*.cpp"
    "game_states/*.cpp"
)

file(GLOB_RECURSE SHADERS_SOURCES
    "shaders/*.cpp"
)

set(ALL_SOURCES
    ${CORE_SOURCES}
    ${UTILITIES_SOURCES}
    ${GAME_LOGIC_SOURCES}
    ${SHADERS_SOURCES}
)

add_executable(beatplate ${ALL_SOURCES})

target_link_libraries(beatplate
    PRIVATE

    optimized "${SFML_LIB_DIR}/sfml-graphics-s.lib"
    optimized "${SFML_LIB_DIR}/sfml-window-s.lib"
    optimized "${SFML_LIB_DIR}/sfml-system-s.lib"
    optimized "${SFML_LIB_DIR}/sfml-audio-s.lib"

    debug "${SFML_LIB_DIR}/sfml-audio-s-d.lib"
    debug "${SFML_LIB_DIR}/sfml-graphics-s-d.lib"
    debug "${SFML_LIB_DIR}/sfml-window-s-d.lib"
    debug "${SFML_LIB_DIR}/sfml-system-s-d.lib"

    "${SFML_LIB_DIR}/flac.lib"
    "${SFML_LIB_DIR}/vorbisenc.lib"
    "${SFML_LIB_DIR}/vorbisfile.lib"
    "${SFML_LIB_DIR}/vorbis.lib"
    "${SFML_LIB_DIR}/ogg.lib"
    "${SFML_LIB_DIR}/freetype.lib"

    opengl32
    winmm
    gdi32
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(beatplate PRIVATE "-mwindows")
endif()

if (MSVC)
    # Suprime o warning LNK4099 no linker
    set_target_properties(beatplate PROPERTIES
        LINK_FLAGS "/ignore:4099"
    )
endif()