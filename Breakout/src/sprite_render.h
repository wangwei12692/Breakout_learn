#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "shader.h"
#include "texture.h"
class SpriteRender
{
public:
	SpriteRender(const Shader& shader);
	~SpriteRender();
	void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f),
		float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f)
		);
private:
	Shader shader;
	unsigned int quadVAO;
	void initRenderData();
};

