#pragma once
#include <vector>
#include <tuple>

#include "game_level.h"
#include "power_up.h"

const glm::vec2 PLAYER_SIZE(100, 20);
const float PLAYER_VELOCITY = 500.0f;
const float BALL_RADIUS = 12.5f;
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game
{
public:
	GameState State;
	std::vector<GameLevel> levels;
	int Level;
	int Lives;
	bool Keys[1024];
	bool KeysProcessed[1024];
	unsigned int Width, Height;
	std::vector<PowerUp> PowerUps;

	Game(unsigned int width,unsigned int height);
	~Game();
	
	void Init();

	//loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void DoCollisions();

	void ResetLevel();
	void ResetPlayer();

	//在给定的砖块位置生产一个道具
	void SpawnPowerUps(GameObject &block);
	//管理更新当前激活的道具
	void UpdatePowerUps(float dt);
};
