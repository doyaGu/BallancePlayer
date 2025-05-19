configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Player.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/Player.rc)

set(PLAYER_HEADERS
        GamePlayer.h
        GameConfig.h
        Splash.h
        ScriptUtils.h
        CmdlineParser.h
        LockGuard.h
        Logger.h
        Utils.h
)

set(PLAYER_SOURCES
        Player.cpp
        GamePlayer.cpp
        GameConfig.cpp
        Hotfix.cpp
        Splash.cpp
        CmdlineParser.cpp
        Logger.cpp
        Utils.cpp
        Player.rc
)

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PLAYER_NAME} PRIVATE CK2 VxMath)

add_executable(ConfigTool
        ConfigTool.cpp
        GameConfig.cpp
        Utils.cpp
        ConfigTool.rc

        ConfigTool.h
        GameConfig.h
        Utils.h
)

set_target_properties(ConfigTool PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_compile_definitions(ConfigTool PRIVATE
        CONFIGTOOL_STANDALONE
)

target_link_libraries(ConfigTool PRIVATE VxMath)

install(TARGETS ${PLAYER_NAME} ConfigTool RUNTIME DESTINATION Bin)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Kakuty")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)