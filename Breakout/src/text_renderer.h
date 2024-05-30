#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

#include "texture.h"
#include "shader.h"

struct Character
{
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

class TextRenderer
{
public:
    std::map<char, Character> Cahracters;
    Shader TextShader;
    TextRenderer(unsigned int width, unsigned int height);
    void Load(std::string font, unsigned int fontSize);
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

private:
    unsigned int VAO, VBO;
};
