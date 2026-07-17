cmake_minimum_required(VERSION 3.10)
project(TimberV2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)

# 链接 SFML 库（添加 sfml-audio）
target_link_libraries(${PROJECT_NAME} PRIVATE
        "/opt/homebrew/lib/libsfml-graphics.dylib"
        "/opt/homebrew/lib/libsfml-window.dylib"
        "/opt/homebrew/lib/libsfml-system.dylib"
        "/opt/homebrew/lib/libsfml-audio.dylib"    # 添加这一行！
)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
        "/opt/homebrew/include"
)

# 设置运行时路径
set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "/opt/homebrew/lib"
        INSTALL_RPATH "/opt/homebrew/lib"
)

# 复制资源文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/graphics
        ${CMAKE_BINARY_DIR}/graphics
        COMMENT "Copying graphics resources to build directory"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/fonts
        ${CMAKE_BINARY_DIR}/fonts
        COMMENT "Copying fonts resources to build directory"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/sound
        ${CMAKE_BINARY_DIR}/sound
        COMMENT "Copying sound resources to build directory"
)