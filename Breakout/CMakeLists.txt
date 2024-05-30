set(CMAKE_CXX_EXTENSIONS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
 set(LIBS glfw3 opengl32 assimp freetype irrKlang)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

#file(COPY "dlls" DESTINATION ${CMAKE_BINARY_DIR})
add_executable (Breakout "src/Breakout.cpp" "src/glad.c" "src/Game.cpp" "src/Game.h"  "src/texture.h" "src/texture.cpp" "src/shader.h" "src/shader.cpp" "src/resource_manager.h" "src/resource_manager.cpp"  "src/stb_image.cpp" "src/sprite_render.h" "src/sprite_render.cpp" "src/game_level.h" "src/game_object.h" "src/game_level.cpp" "src/game_object.cpp" "src/ball_object.h" "src/ball_object.cpp" "src/particle_generator.h" "src/particle_generator.cpp" "src/post_processing.h" "src/post_processing.cpp" "src/power_up.h" "src/text_renderer.h" "src/text_renderer.cpp")
target_link_libraries(Breakout ${LIBS})

file(GLOB DLLS "dlls/*.dll")
add_custom_command(TARGET Breakout PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLLS} $<TARGET_FILE_DIR:Breakout>)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Breakout PROPERTY CXX_STANDARD 20)
endif()

