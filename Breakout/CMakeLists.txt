set(CMAKE_CXX_EXTENSIONS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(LIBS glfw3 opengl32)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable (Breakout "src/Breakout.cpp" "src/glad.c" "src/Game.cpp" "src/Game.h"  "src/texture.h" "src/texture.cpp" "src/shader.h" "src/shader.cpp" "src/resource_manager.h" "src/resource_manager.cpp"  "src/stb_image.cpp" "src/sprite_render.h" "src/sprite_render.cpp" "src/game_level.h" "src/game_object.h" "src/game_level.cpp" "src/game_object.cpp" "src/ball_object.h" "src/ball_object.cpp")
target_link_libraries(Breakout ${LIBS})
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Breakout PROPERTY CXX_STANDARD 20)
endif()

