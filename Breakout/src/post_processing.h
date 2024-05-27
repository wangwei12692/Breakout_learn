#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "sprite_render.h"

class PostProcessor
{
public:
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;
	bool Confuse, Chaos, Shake;
	PostProcessor(const Shader& shader,unsigned int width,unsigned int height);
	void BeginRender();
	void EndRender();
	void Render(float time);
private:
	unsigned int MSFBO, FBO; //Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
	unsigned int RBO;//multisamplered color buffer
	unsigned int VAO;
	void initRenderData();
};
