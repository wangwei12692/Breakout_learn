#include <glm/glm.hpp>
#include <iostream>
//先引入glad，在引入glfw，不然报错
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "resource_manager.h"
#include "sprite_render.h"
#include "game_object.h"
#include "ball_object.h"


SpriteRender* Renderer;
GameObject* Player;
BallObject* Ball;

Game::Game(unsigned int width, unsigned int height)
	:State(GAME_ACTIVE), Keys(), KeysProcessed(), Width(width), Height(height), Level(0)
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
	ResourceManager::LoadTexture("Breakout/resources/textures/face.png", true, "face");
	ResourceManager::LoadTexture("Breakout/resources/textures/background.png", false, "background");
	ResourceManager::LoadTexture("Breakout/resources/textures/block.png", false, "block");
	ResourceManager::LoadTexture("Breakout/resources/textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("Breakout/resources/textures/paddle.png", true, "paddle");
	//load levels

	GameLevel one;one.Load("Breakout/resources/levels/one.lvl", this->Width, this->Height * 0.5f);
	GameLevel two;two.Load("Breakout/resources/levels/two.lvl", this->Width, this->Height * 0.5f);
	GameLevel three;three.Load("Breakout/resources/levels/three.lvl", this->Width, this->Height * 0.5f);
	GameLevel four;four.Load("Breakout/resources/levels/four.lvl", this->Width, this->Height * 0.5f);
	this->levels.push_back(one);
	this->levels.push_back(two);
	this->levels.push_back(three);
	this->levels.push_back(four);
	this->Level = 0;

	auto playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x /2,
		this->Height -  PLAYER_SIZE.y
	);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	auto ballPos = playerPos + glm::vec2(PLAYER_SIZE.x /2.0f - BALL_RADIUS,-BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos,BALL_RADIUS, INITIAL_BALL_VELOCITY,ResourceManager::GetTexture("face"));
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_ACTIVE) {
		auto velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0) {
				Player->Position.x -= velocity;
				if (Ball->Stuck) {
					Ball->Position.x -= velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x) {
				Player->Position.x += velocity;
				if (Ball->Stuck) {
					Ball->Position.x += velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
		{
			Ball->Stuck = false;
		}
	}
}

void Game::Update(float dt)
{
	Ball->Move(dt, this->Width);
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE) {
		//draw background
		auto background = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(background, glm::vec2(0, 0), glm::vec2(this->Width, this->Height));

		//draw level
		this->levels[this->Level].Draw(*Renderer);

		//drwa paddle
		Player->Draw(*Renderer);
		//draw ball
		Ball->Draw(*Renderer);
			
	}
	
}
