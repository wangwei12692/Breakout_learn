#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
// 先引入glad，在引入glfw，不然报错
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <irrklang/irrKlang.h>

#include "Game.h"
#include "resource_manager.h"
#include "sprite_render.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processing.h"
#include "text_renderer.h"

using namespace irrklang;

SpriteRender *Renderer;
PostProcessor *Effects;
ParticleGenerator *Particles;
GameObject *Player;
BallObject *Ball;
ISoundEngine *SoundEngine = createIrrKlangDevice();
TextRenderer* Text;

float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE), Keys(), KeysProcessed(), Width(width), Height(height), Level(0),Lives(3)
{
}

Game::~Game()
{
	delete Renderer;
	delete Effects;
	delete Particles;
	delete Player;
	delete Ball;
	SoundEngine->drop();
}

void Game::Init()
{
	// load shaders
	ResourceManager::LoadShader("Breakout/resources/sprite.vs",
								"Breakout/resources/sprite.fs", nullptr, "sprite");
	ResourceManager::LoadShader("Breakout/resources/particle.vs",
								"Breakout/resources/particle.fs", nullptr, "particle");
	ResourceManager::LoadShader("Breakout/resources/post_processing.vs",
								"Breakout/resources/post_processing.fs", nullptr, "post_processing");
	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);
	// load textures
	ResourceManager::LoadTexture("Breakout/resources/textures/face.png", true, "face");
	ResourceManager::LoadTexture("Breakout/resources/textures/background.png", false, "background");
	ResourceManager::LoadTexture("Breakout/resources/textures/block.png", false, "block");
	ResourceManager::LoadTexture("Breakout/resources/textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("Breakout/resources/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("Breakout/resources/textures/particle.png", true, "particle");
	ResourceManager::LoadTexture("Breakout/resources/textures/powerup_speed.png", true, "powerup_speed");
    ResourceManager::LoadTexture("Breakout/resources/textures/powerup_sticky.png", true, "powerup_sticky");
    ResourceManager::LoadTexture("Breakout/resources/textures/powerup_increase.png", true, "powerup_increase");
    ResourceManager::LoadTexture("Breakout/resources/textures/powerup_confuse.png", true, "powerup_confuse");
    ResourceManager::LoadTexture("Breakout/resources/textures/powerup_chaos.png", true, "powerup_chaos");
    ResourceManager::LoadTexture("Breakout/resources/textures/powerup_passthrough.png", true, "powerup_passthrough");

	Renderer = new SpriteRender(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	Effects = new PostProcessor(ResourceManager::GetShader("post_processing"), this->Width, this->Height);
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("Breakout/resources/fonts/OCRAEXT.TTF",24);
	// load levels
	GameLevel one;
	one.Load("Breakout/resources/levels/one.lvl", this->Width, this->Height * 0.5f);
	GameLevel two;
	two.Load("Breakout/resources/levels/two.lvl", this->Width, this->Height * 0.5f);
	GameLevel three;
	three.Load("Breakout/resources/levels/three.lvl", this->Width, this->Height * 0.5f);
	GameLevel four;
	four.Load("Breakout/resources/levels/four.lvl", this->Width, this->Height * 0.5f);
	this->levels.push_back(one);
	this->levels.push_back(two);
	this->levels.push_back(three);
	this->levels.push_back(four);
	this->Level = 0;

	auto playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	auto ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

	SoundEngine->play2D("Breakout/resources/audio/breakout.mp3",true);
}

void Game::ProcessInput(float dt)
{

	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = true;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			//this->Level = (this->Level - 1) % 4;
			this->KeysProcessed[GLFW_KEY_S] = true;
		}
	}
	else if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Chaos = false;
			this->State = GAME_MENU;
		}
	}
	else if (this->State == GAME_ACTIVE)
	{
		auto velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
				{
					Ball->Position.x -= velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
				{
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
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
	this->UpdatePowerUps(dt);
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
		{
			Effects->Shake = false;
		}
	}
	if (Ball->Position.y >= this->Height)
	{
		--this->Lives;
		if (this->Lives == 0) {
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	if (this->State == GAME_ACTIVE && this->levels[this->Level].IsComplete()) {
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = true;
		this->State = GAME_WIN;
	}

}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{

		// draw to framebuffer
		Effects->BeginRender();
		// draw background
		auto background = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(background, glm::vec2(0, 0), glm::vec2(this->Width, this->Height));

		// draw level
		this->levels[this->Level].Draw(*Renderer);

		// draw paddle
		Player->Draw(*Renderer);
		for (auto &powerUp : this->PowerUps)
		{
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		}
		Particles->Draw();
		// draw ball
		Ball->Draw(*Renderer);
		Effects->EndRender();
		Effects->Render(glfwGetTime());
		std::stringstream ss;
		ss << this -> Lives;
		Text->RenderText("Lives: " + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	else if (this->State == GAME_MENU) {
		Text->RenderText("Press ENTER to start", 250.0f, this->Height / 2.0f, 1.0f);
		Text->RenderText("Press W or S to select level", 245.0f, this->Height / 2.0f + 20.0f, 0.75f);
	}
	else {
		Text->RenderText("You WON!!!", 320.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
	}
}

bool CheckCollision(GameObject &one, GameObject &two);
Collision CheckCollision(BallObject &one, GameObject &two);
void ActivePowerUp(PowerUp &powerUp);

void Game::DoCollisions()
{
	for (auto &box : this->levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			auto collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))
			{
				if (!box.IsSolid)
				{
					box.Destroyed = true;
					this->SpawnPowerUps(box);
					SoundEngine->play2D("Breakout/resources/audio/bleep.mp3",false);
				}
				else
				{
					//if block is solid,enable shake effect
					ShakeTime = 0.05f;
					Effects->Shake = true;
					SoundEngine->play2D("Breakout/resources/audio/bleep.mp3",false);
				}
				auto dir = std::get<1>(collision);
				auto diff_vector = std::get<2>(collision);
				if (!(Ball->PassThrouth && !box.IsSolid))
				{
					if (dir == LEFT || dir == RIGHT)
					{
						// 反转水平速度
						Ball->Velocity.x = -Ball->Velocity.x;
						// 重定位
						float pentration = Ball->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
						{
							Ball->Position.x += pentration;
						}
						else
						{
							Ball->Position.x -= pentration;
						}
					}
					else
					{
						// 反转垂直速度
						Ball->Velocity.y = -Ball->Velocity.y;
						// 重定位
						float pentration = Ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
						{
							Ball->Position.y += pentration;
						}
						else
						{
							Ball->Position.y -= pentration;
						}
					}
				}
			}
		}
	}
	auto result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// 挡板中心
		auto centerBoard = Player->Position.x + Player->Size.x / 2;
		auto distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		auto percentage = distance / (Player->Size.x / 2);

		auto strength = 2.0f;
		auto oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -Ball->Velocity.y;
		// 保持球的力量和速度一致，不然可能x方向越来越大；
		// 报纸x与y 加以来一致，x如果大了就缩小y
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		// 解决粘板问题（小球速度过快，在挡板内多次反转，这样子直接返回负数，假定碰撞在顶部）
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);

		Ball->Stuck = Ball->Sticky;
		SoundEngine->play2D("Breakout/resources/audio/bleep.wav",false);
	}

	for (auto &powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)
			{
				powerUp.Destroyed = true;
			}
			if (CheckCollision(*Player, powerUp))
			{
				ActivePowerUp(powerUp);
				powerUp.Destroyed = true;
				powerUp.Activated = true;
				SoundEngine->play2D("Breakout/resources/audio/powerup.wav",false);
			}
		}
	}
}

void Game::ResetLevel()
{
	if (this->Level == 0)
	{
		this->levels[0].Load("Breakout/resources/levels/one.lvl", this->Width, this->Height * 0.5f);
	}
	else if (this->Level == 0)
	{
		this->levels[1].Load("Breakout/resources/levels/two.lvl", this->Width, this->Height * 0.5f);
	}
	else if (this->Level == 0)
	{
		this->levels[2].Load("Breakout/resources/levels/three.lvl", this->Width, this->Height * 0.5f);
	}
	else if (this->Level == 0)
	{
		this->levels[3].Load("Breakout/resources/levels/four.lvl", this->Width, this->Height * 0.5f);
	}
	this->Lives = 3;
}

void Game::ResetPlayer()
{
	auto playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y);

	Player->Position = playerPos;
	auto ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball->Reset(ballPos, INITIAL_BALL_VELOCITY);
	
	Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
	Effects->Chaos = Effects->Confuse = false;
}

bool shouldSpawn(unsigned int chance)
{
	auto random = rand() % chance;
	return random == 0;
}

// 在给定的砖块位置生产一个道具
void Game::SpawnPowerUps(GameObject &block)
{
	if (shouldSpawn(75))
	{
		this->PowerUps.push_back(
			PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position,
					ResourceManager::GetTexture("powerup_speed")));
	}
	if (shouldSpawn(75))
	{
		this->PowerUps.push_back(
			PowerUp("sticky", glm::vec3(0.5f, 0.5f, 1.0f), 20.0f, block.Position,
					ResourceManager::GetTexture("powerup_sticky")));
	}
	if (shouldSpawn(75))
	{
		this->PowerUps.push_back(
			PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position,
					ResourceManager::GetTexture("powerup_passthrough")));
	}
	if (shouldSpawn(75))
	{
		this->PowerUps.push_back(
			PowerUp("pad-size-incerease", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position,
					ResourceManager::GetTexture("powerup_increase")));
	}
	if (shouldSpawn(15))
	{
		this->PowerUps.push_back(
			PowerUp("confuse", glm::vec3(0.5f, 0.5f, 1.0f), 15.0f, block.Position,
					ResourceManager::GetTexture("powerup_confuse")));
	}
	if (shouldSpawn(15))
	{
		this->PowerUps.push_back(
			PowerUp("chaos", glm::vec3(0.5f, 0.5f, 1.0f), 15.0f, block.Position,
					ResourceManager::GetTexture("powerup_chaos")));
	}
}

bool isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	for (auto &powerUp : powerUps)
	{
		if (powerUp.Activated)
		{
			if (powerUp.Type == type)
			{
				return true;
			}
		}
	}
	return false;
}

// 每次update的时候调用
// 设置运动，看看是否没有激活了
void Game::UpdatePowerUps(float dt)
{
	for (auto &&powerUp : PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;
			if (powerUp.Duration <= 0.0f)
			{
				powerUp.Activated = false;
				if (powerUp.Type == "sticky")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "sticky"))
					{
						Ball->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "pass-through"))
					{
						Ball->PassThrouth = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "confuse"))
					{
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
					{
						Effects->Chaos = false;
					}
				}
			}
		}
	}
	this->PowerUps.erase(
		std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
					   [](const PowerUp &powerUp)
					   { return powerUp.Destroyed && !powerUp.Activated; }),
		this->PowerUps.end());
}

bool CheckCollision(GameObject &one, GameObject &two)
{
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;

	return collisionX && collisionY;
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// UP
		glm::vec2(1.0f, 1.0f),	// RIGHT
		glm::vec2(0.0f, -1.0f), // DOWN
		glm::vec2(-1.0f, 1.0f), // LEFT
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

Collision CheckCollision(BallObject &one, GameObject &two)
{
	glm::vec2 center(one.Position.x, one.Position.y);
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y);
	// 差失量
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	auto closest = aabb_center + clamped;
	difference = closest - center;
	if (glm::length(difference) <= one.Radius)
	{
		return std::make_tuple(true, VectorDirection(difference), difference);
	}
	else
	{
		return std::make_tuple(false, UP, glm::vec2(0, 0));
	}
}

// 设置属性为激活
void ActivePowerUp(PowerUp &powerUp)
{
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2f;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrouth = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
		{
			Effects->Confuse = true;
		}
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
		{
			Effects->Chaos = true;
		}
	}
}