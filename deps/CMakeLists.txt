# cxxopts
add_subdirectory(cxxopts EXCLUDE_FROM_ALL)

# zip
add_subdirectory(zip EXCLUDE_FROM_ALL)

# yyjson
add_subdirectory(yyjson EXCLUDE_FROM_ALL)

# utf8.h
add_library(utf8 INTERFACE)
target_include_directories(utf8 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/utf8.h)

# ImGui
set(IMGUI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui PARENT_SCOPE)
