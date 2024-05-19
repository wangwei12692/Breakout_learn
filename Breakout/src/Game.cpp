#include <glm/glm.hpp>
#include <iostream>


#include "Game.h"
#include "resource_manager.h"
#include "sprite_render.h"

SpriteRender* Renderer;

Game::Game(unsigned int width, unsigned int height)
	:State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete Renderer;
}

void Game::Init()
{
	auto shader = ResourceManager::LoadShader("Breakout/resources/sprite.vs", 
		"Breakout/resources/sprite.fs", nullptr, "sprite");

	glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
	shader.Use().SetInteger("image", 0);
	shader.Use().SetMatrix4("projection", projection);

	Renderer = new SpriteRender(shader);

	//load texture
	ResourceManager::LoadTexture("Breakout/resources/textures/face.png", GL_TRUE, "face");
}

void Game::ProcessInput(float dt)
{
}

void Game::Update(float dt)
{
}

void Game::Render()
{
	
	auto texture = ResourceManager::GetTexture("face");
	Renderer->DrawSprite(texture, glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));

}
