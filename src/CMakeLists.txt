set(PLAYER_NAME "Player")
set(LIBRARY_NAME "Game")

set(BP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${BP_INCLUDE_DIR}/bp/Version.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Defines.h.in ${BP_INCLUDE_DIR}/bp/Defines.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Game.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/Game.rc)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Player.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/Player.rc)

set(LIBRARY_HEADERS
        GamePlayer.h
        GameManager.h
        GameConfig.h
        ScriptUtils.h
        Window.h
        Configuration.h
        DataShare.h
        Logger.h
        Variant.h
)

set(LIBRARY_SOURCES
        Game.cpp
        GamePlayer.cpp
        GameManager.cpp
        GameConfig.cpp
        Hotfix.cpp
        Configuration.cpp
        DataShare.cpp
        Logger.cpp
        Variant.cpp
        Utils.cpp
        Game.rc
)

set(IMGUI_HEADERS
        ${IMGUI_SOURCE_DIR}/imgui.h
        ${IMGUI_SOURCE_DIR}/imgui_internal.h
        ${IMGUI_SOURCE_DIR}/imstb_rectpack.h
        ${IMGUI_SOURCE_DIR}/imstb_textedit.h
        ${IMGUI_SOURCE_DIR}/imstb_truetype.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_ck2.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cimgui.h
)

set(IMGUI_SOURCES
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
        ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
        ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_ck2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cimgui.cpp
)

if (BP_ENABLE_IMGUI)
    list(APPEND LIBRARY_HEADERS ${IMGUI_HEADERS})
    list(APPEND LIBRARY_SOURCES ${IMGUI_SOURCES})
endif ()

add_library(${LIBRARY_NAME} SHARED
        ${LIBRARY_HEADERS} ${LIBRARY_SOURCES}
)

target_include_directories(${LIBRARY_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${BP_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (BP_ENABLE_IMGUI)
    target_include_directories(${LIBRARY_NAME} PUBLIC
            $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>
    )
endif ()

target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        CK2 VxMath
        PRIVATE
        zip yyjson utf8
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE "BP_EXPORTS")

if (BP_ENABLE_IMGUI)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE "IMGUI_EXPORT")
endif ()

add_custom_command(TARGET ${LIBRARY_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h" ${IMGUI_SOURCE_DIR}
        VERBATIM
)

add_executable(${PLAYER_NAME} WIN32
        Player.cpp
        Splash.cpp
        Splash.h
        Player.rc
)

target_include_directories(${PLAYER_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${BP_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PLAYER_NAME}
        PRIVATE ${LIBRARY_NAME} cxxopts
)

install(TARGETS ${PLAYER_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(TARGETS ${LIBRARY_NAME} EXPORT BallanceTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT BallanceTargets
        NAMESPACE Ballance::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Ballance")

install(DIRECTORY "${BP_INCLUDE_DIR}/bp" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (BP_ENABLE_IMGUI)
    install(FILES
            "${IMGUI_SOURCE_DIR}/imconfig.h"
            "${IMGUI_SOURCE_DIR}/imgui.h"
            "${IMGUI_SOURCE_DIR}/imgui_internal.h"
            "${IMGUI_SOURCE_DIR}/imstb_rectpack.h"
            "${IMGUI_SOURCE_DIR}/imstb_textedit.h"
            "${IMGUI_SOURCE_DIR}/imstb_truetype.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/cimgui.h"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif ()

configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/BallanceConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/BallanceConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Ballance"
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/BallanceConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/BallanceConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BallanceConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Ballance"
)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Kakuty")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_GENERATOR "ZIP")
include(CPack)