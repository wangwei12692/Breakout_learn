#pragma once
#include <vector>
#include "game_level.h"


const glm::vec2 PLAYER_SIZE(100, 20);
const float PLAYER_VELOCITY = 500.0f;
const float BALL_RADIUS = 12.5f;
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game
{
public:
	GameState State;
	std::vector<GameLevel> levels;
	int Level;
	bool Keys[1024];
	bool KeysProcessed[1024];
	unsigned int Width, Height;

	Game(unsigned int width,unsigned int height);
	~Game();
	
	void Init();

	//loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
};
