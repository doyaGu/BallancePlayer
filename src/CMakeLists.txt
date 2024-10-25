if (NOT DEFINED PLAYER_NAME)
    set(PLAYER_NAME "Player")
endif ()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Player.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/Player.rc)

set(PLAYER_HEADERS
        GamePlayer.h
        GameConfig.h
        Window.h
        Splash.h
        ScriptUtils.h
        ConsoleAttacher.h
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
        Logger.cpp
        Utils.cpp
        Player.rc
        )

set(IMGUI_HEADERS
        ${IMGUI_SOURCE_DIR}/imgui.h
        ${IMGUI_SOURCE_DIR}/imgui_internal.h
        ${IMGUI_SOURCE_DIR}/imstb_rectpack.h
        ${IMGUI_SOURCE_DIR}/imstb_textedit.h
        ${IMGUI_SOURCE_DIR}/imstb_truetype.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_ck2.h
)

set(IMGUI_SOURCES
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
        ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
        ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_ck2.cpp
)

add_executable(${PLAYER_NAME}
        ${PLAYER_HEADERS} ${PLAYER_SOURCES}
        ${IMGUI_SOURCES} ${IMGUI_HEADERS}
)

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        WIN32_EXECUTABLE TRUE
        )

target_include_directories(${PLAYER_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PLAYER_NAME}
        PUBLIC
        CK2 VxMath
        PRIVATE
        cxxopts
)

add_custom_command(TARGET ${PLAYER_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h" "${IMGUI_SOURCE_DIR}"
        VERBATIM
)

install(TARGETS ${PLAYER_NAME} RUNTIME DESTINATION Bin)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Kakuty")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)