
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.h"
#include "resource_manager.h"

TextRenderer::TextRenderer(unsigned int width, unsigned int height)
{
    this->TextShader = ResourceManager::LoadShader("Breakout/resources/text_2d.vs", "Breakout/resources/text_2d.fs", nullptr, "");
    // 设置投影矩阵
    this->TextShader.SetMatrix4("projection",
                                glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f),
                                true);
    // 设置纹理绑定位置
    this->TextShader.SetInteger("text", 0);

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    // 每个字 由 6 个顶点坐标，每个4个（xy,zw）（位置，纹理）决定
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, unsigned int fontSize)
{
    this->Cahracters.clear();
    FT_Library ft;
    if(FT_Init_FreeType(&ft)){
        std::cout << "ERROR:FREETYPE:Cloud not init FreeType Library" << std::endl;
    }
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face)) {
        std::cout << "ERROR:FREETYPE:Failed to load font" << std::endl;
    }
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    //
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (auto c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face,c,FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        

        Character character = {
            texture, //texture
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), //size 
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), //bearing 
            face->glyph->advance.x 
        };
        Cahracters.insert(std::make_pair(c, character));
    }
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    this->TextShader.Use();
    this->TextShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    for(auto &c : text){
        auto ch = this->Cahracters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (this->Cahracters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        //为什么写这一步？
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // 更新位置到下一个字形的原点，注意单位是1/64像素
        x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
