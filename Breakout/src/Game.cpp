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
	this->DoCollisions();
	if (Ball->Position.y >= this->Height)
	{
		this->ResetLevel();
		this->ResetPlayer();
	}
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

bool CheckCollision(GameObject& one,GameObject& two);
Collision CheckCollision(BallObject& one,GameObject& two);

void Game::DoCollisions()
{
	for(auto &box : this->levels[this->Level].Bricks) {
		if (!box.Destroyed) {
			auto collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))
			{
				if (!box.IsSolid) {
					box.Destroyed = true;
				}
				auto dir = std::get<1>(collision);
				auto diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT)
				{
					//反转水平速度
					Ball->Velocity.x = -Ball->Velocity.x;
					//重定位
					float pentration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
					{
						Ball->Position.x += pentration;
					}
					else {
						Ball->Position.x -= pentration;
					}
				}
				else {
					//反转垂直速度
					Ball->Velocity.y = -Ball->Velocity.y;
					//重定位
					float pentration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == UP)
					{
						Ball->Position.y += pentration;
					}
					else {
						Ball->Position.y -= pentration;
					}
										
				}
			}
		}
	}
	auto result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		//挡板中心
		auto centerBoard = Player->Position.x + Player->Size.x / 2;
		auto distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		auto percentage = distance / (Player->Size.x / 2);

		auto strength = 2.0f;
		auto oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -Ball->Velocity.y;
		//保持球的力量和速度一致，不然可能x方向越来越大；
		//报纸x与y 加以来一致，x如果大了就缩小y
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		//解决粘板问题（小球速度过快，在挡板内多次反转，这样子直接返回负数，假定碰撞在顶部）
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
	}
}

void Game::ResetLevel()
{
	if (this->Level == 0)
	{
		this->levels[0].Load("Breakout/resources/levels/one.lvl", this->Width, this->Height * 0.5f);
	}else if (this->Level == 0)
	{
		this->levels[1].Load("Breakout/resources/levels/two.lvl", this->Width, this->Height * 0.5f);
	}else if (this->Level == 0)
	{
		this->levels[2].Load("Breakout/resources/levels/three.lvl", this->Width, this->Height * 0.5f);
	}else if (this->Level == 0)
	{
		this->levels[3].Load("Breakout/resources/levels/four.lvl", this->Width, this->Height * 0.5f);
	}
}

void Game::ResetPlayer()
{
	auto playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);
	
	Player->Position = playerPos;
	auto ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball->Reset(ballPos, INITIAL_BALL_VELOCITY);
}

bool CheckCollision(GameObject& one, GameObject& two) {
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x
		&& two.Position.x + two.Size.x >= one.Position.x;
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y
		&& two.Position.y + two.Size.y >= one.Position.y;

	return collisionX && collisionY;
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f,1.0f),//UP
		glm::vec2(1.0f,1.0f),//RIGHT
		glm::vec2(0.0f,-1.0f),//DOWN
		glm::vec2(-1.0f,1.0f),//LEFT
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (int i = 0;i < 4;i++) {
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}


Collision CheckCollision(BallObject& one, GameObject& two) {
	glm::vec2 center(one.Position.x,one.Position.y);
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x, 
		two.Position.y + aabb_half_extents.y
	);
	//差失量
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	auto closest = aabb_center + clamped;
	difference = closest - center;
	if (glm::length(difference) <= one.Radius)
	{
		return std::make_tuple(true, VectorDirection(difference), difference);
	}
	else {
		return std::make_tuple(false, UP, glm::vec2(0, 0));
	}
}
