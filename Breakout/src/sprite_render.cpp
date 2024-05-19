#include "sprite_render.h"

SpriteRender::SpriteRender(Shader& shader)
{
	this->shader = shader;
	this->initRenderData();
}

SpriteRender::~SpriteRender()
{
	glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRender::DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
	this->shader.Use();
	glm::mat4 translateMat(1.0f);
	//二维坐标（pos.x,pos.y,0)
	//scale -> rotation -> translate
	translateMat = glm::translate(translateMat, glm::vec3(position, 0.0f));

	glm::mat4 rotateMat(1.0f);
	//改变旋转的原点到正方体中心 center = (size.x * 0.5,size.y * 0.5)
	rotateMat = glm::translate(rotateMat, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	//围绕z轴旋转
	rotateMat = glm::rotate(rotateMat, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	//复原
	rotateMat = glm::translate(rotateMat, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	//放大至size 大小，从左上角
	glm::mat4 scaleMat(1.0f);
	scaleMat = glm::scale(scaleMat, glm::vec3(size, 1.0f));

	this->shader.SetMatrix4("model", translateMat * rotateMat * scaleMat);
	this->shader.SetVector3f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRender::initRenderData()
{
	//一组以四边形的左上角为(0,0)坐标的顶点。这意味着当我们在四边形上应用一个位移或缩放变换的时候，它们会从四边形的左上角开始进行变换。这在2D图形以及/或GUI系统中广为接受，元素的位置定义为元素左上角的位置。
	unsigned int VBO;
	float vertices[] = {
		//pos     //tex
		0.0f,1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
