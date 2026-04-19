include(FetchContent)
set(_local_googletest "${PROJECT_SOURCE_DIR}/build/_deps/googletest-src")
if (EXISTS "${_local_googletest}/CMakeLists.txt")
    set(FETCHCONTENT_SOURCE_DIR_GOOGLETEST "${_local_googletest}" CACHE PATH "" FORCE)
endif ()
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

function(add_player_test test_name)
    cmake_parse_arguments(TEST "" "" "SOURCES;DEPENDENCIES" ${ARGN})

    add_executable(${test_name} ${TEST_SOURCES})
    target_compile_features(${test_name} PUBLIC cxx_std_17)

    target_include_directories(${test_name} PRIVATE
            "${PLAYER_INCLUDE_DIR}"
            "${PLAYER_SOURCE_DIR}"
    )

    target_link_libraries(${test_name} PRIVATE
            gtest_main
            gmock
            ${TEST_DEPENDENCIES}
    )

    target_compile_definitions(${test_name} PRIVATE "PLAYER_TEST")

    set_target_properties(${test_name} PROPERTIES FOLDER "Tests")

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_player_test(GameConfigTest
        SOURCES GameConfigTest.cpp
        ${PLAYER_SOURCE_DIR}/GameConfig.cpp
        ${PLAYER_SOURCE_DIR}/Utils.cpp
        DEPENDENCIES VxMath
)

add_player_test(UtilsTest
        SOURCES UtilsTest.cpp
        ${PLAYER_SOURCE_DIR}/Utils.cpp
        DEPENDENCIES VxMath
)

add_player_test(PlayerOptionsTest
        SOURCES PlayerOptionsTest.cpp
        ${PLAYER_SOURCE_DIR}/PlayerOptions.cpp
        ${PLAYER_SOURCE_DIR}/CmdlineParser.cpp
        ${PLAYER_SOURCE_DIR}/GameConfig.cpp
        ${PLAYER_SOURCE_DIR}/Utils.cpp
        DEPENDENCIES VxMath
)

add_player_test(CmdlineParserTest
        SOURCES CmdlineParserTest.cpp
        ${PLAYER_SOURCE_DIR}/CmdlineParser.cpp
)
