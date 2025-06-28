cmake_minimum_required(VERSION 3.16)
project(beatplate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DSFML_STATIC)

include_directories(
    utils
    shaders
    libs
    game_states
    game_logic
    game_logic/note_objects
    ${CMAKE_SOURCE_DIR}/libs/SFML-3.0.0/include
)

file(GLOB_RECURSE SOURCES
    "main.cpp"
    "state_stack.cpp"
    "utils/*.cpp"
    "game_logic/*.cpp"
    "game_logic/note_objects/*.cpp"
    "game_states/*.cpp"
    "shaders/*.cpp"
)

add_executable(beatplate ${SOURCES})

# Construindo os caminhos absolutos para as libs (ajuste se seu caminho for diferente)
set(SFML_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/SFML-3.0.0/lib")

target_link_libraries(beatplate
    optimized "${SFML_LIB_DIR}/sfml-graphics-s.lib"
    debug    "${SFML_LIB_DIR}/sfml-graphics-s-d.lib"
    optimized "${SFML_LIB_DIR}/sfml-window-s.lib"
    debug    "${SFML_LIB_DIR}/sfml-window-s-d.lib"
    optimized "${SFML_LIB_DIR}/sfml-system-s.lib"
    debug    "${SFML_LIB_DIR}/sfml-system-s-d.lib"
    optimized "${SFML_LIB_DIR}/sfml-audio-s.lib"
    debug    "${SFML_LIB_DIR}/sfml-audio-s-d.lib"

    "${SFML_LIB_DIR}/freetype.lib"
    opengl32
    winmm
    gdi32
    "${SFML_LIB_DIR}/flac.lib"
    "${SFML_LIB_DIR}/vorbisenc.lib"
    "${SFML_LIB_DIR}/vorbisfile.lib"
    "${SFML_LIB_DIR}/vorbis.lib"
    "${SFML_LIB_DIR}/ogg.lib"
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
