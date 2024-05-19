#pragma once
#include <vector>
#include "game_level.h"

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
