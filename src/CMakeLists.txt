set(_player_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${_player_generated_dir}/config.h"
        @ONLY
)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/Player.rc.in"
        "${_player_generated_dir}/Player.rc"
        @ONLY
)

set(CONFIG_TOOL_LAUNCHERS
        "${CMAKE_CURRENT_SOURCE_DIR}/ConfigTool.bat"
        "${CMAKE_CURRENT_SOURCE_DIR}/ConfigTool.vbs"
)

set(PLAYER_HEADERS
        GamePlayer.h
        GameConfig.h
        PlayerOptions.h
        ConfigDialog.h
        Splash.h
        ScriptUtils.h
        CmdlineParser.h
        LockGuard.h
        Logger.h
        Utils.h
)

set(PLAYER_SOURCES
        Player.cpp
        ConfigDialog.cpp
        GamePlayer.cpp
        GameConfig.cpp
        PlayerOptions.cpp
        Hotfix.cpp
        Splash.cpp
        CmdlineParser.cpp
        Logger.cpp
        Utils.cpp
        "${_player_generated_dir}/Player.rc"
)

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PLAYER_NAME} PRIVATE CK2 VxMath)

target_include_directories(${PLAYER_NAME} PRIVATE
        "${_player_generated_dir}"
)

add_custom_command(TARGET ${PLAYER_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                ${CONFIG_TOOL_LAUNCHERS}
                "$<TARGET_FILE_DIR:${PLAYER_NAME}>"
        COMMENT "Copying ConfigTool compatibility launchers"
)

install(TARGETS ${PLAYER_NAME} RUNTIME DESTINATION Bin)
install(FILES ${CONFIG_TOOL_LAUNCHERS} DESTINATION Bin)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Kakuty")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)
