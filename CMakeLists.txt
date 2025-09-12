cmake_minimum_required(VERSION 3.16)
    project(beatplate)

    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    add_definitions(-DSFML_STATIC)

    include_directories(
        ${CMAKE_SOURCE_DIR}
        utils
        shaders
        libs
        game_states
        game_logic
        game_logic/note_objects
        ${CMAKE_SOURCE_DIR}/libs/SFML-3.0.0/include
    )

    # --- Recolhe todos os fontes do projeto (exceto os que vamos mover para 'core')
    file(GLOB_RECURSE ALL_SOURCES
        "${CMAKE_SOURCE_DIR}/main.cpp"
        "${CMAKE_SOURCE_DIR}/state_stack.cpp"
        "${CMAKE_SOURCE_DIR}/utils/*.cpp"
        "${CMAKE_SOURCE_DIR}/game_logic/*.cpp"
        "${CMAKE_SOURCE_DIR}/game_logic/note_objects/*.cpp"
        "${CMAKE_SOURCE_DIR}/game_states/*.cpp"
        "${CMAKE_SOURCE_DIR}/shaders/*.cpp"
    )

    # --- Defina aqui os arquivos que compõem a lógica 'core' (a ser testada).
    # Ajuste/adicione arquivos conforme necessário. Ex.: game_logic/song_select.cpp
    set(CORE_SOURCES
        "${CMAKE_SOURCE_DIR}/game_states/song_select.cpp"
        # adicione outros .cpp que você quer compilar dentro da lib core
        # "${CMAKE_SOURCE_DIR}/utils/alguma_coisa.cpp"
    )

    # Remove os fontes de 'core' da lista do executável principal para evitar double-compile
    list(REMOVE_ITEM ALL_SOURCES ${CORE_SOURCES})

    # --- Biblioteca core (código testável)
    add_library(core STATIC ${CORE_SOURCES})
    target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR})

    # --- Executável principal (mantendo o comportamento original)
    add_executable(beatplate ${ALL_SOURCES})

    # Construindo os caminhos absolutos para as libs (ajuste se seu caminho for diferente)
    set(SFML_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/SFML-3.0.0/lib")

    # Link: linkamos beatplate com core e com as libs SFML que já tinha
    target_link_libraries(beatplate
        PRIVATE
        core
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

    # Se quiser que core também use libs, linke aqui. Exemplo:
    # target_link_libraries(core PRIVATE ...)

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_options(beatplate PRIVATE "-mwindows")
    endif()

    if (MSVC)
        # Suprime o warning LNK4099 no linker
        set_target_properties(beatplate PROPERTIES
            LINK_FLAGS "/ignore:4099"
        )
    endif()

    # -------------------
    # --- Unit tests ----
    # -------------------
    option(BUILD_TESTS "Build unit tests" ON)

    if(BUILD_TESTS)
        enable_testing()

        # Use file(GLOB_RECURSE ...) para coletar todos os arquivos de teste
        file(GLOB_RECURSE TEST_SOURCES
            "${CMAKE_SOURCE_DIR}/tests/*.cpp"
            "${CMAKE_SOURCE_DIR}/libs/catch_amalgamated.cpp"
        )

        add_executable(unit_tests ${TEST_SOURCES})
        target_include_directories(unit_tests PRIVATE 
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/tests
        )
        target_link_libraries(unit_tests PRIVATE core)

        add_test(NAME AllTests COMMAND unit_tests)
    endif()
