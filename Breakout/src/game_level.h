#pragma once
#include <vector>

#include "game_object.h"
#include "sprite_render.h"


class GameLevel
{
public:
	std::vector<GameObject> Bricks;
	GameLevel(){}

	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
	//渲染关卡
	void Draw(SpriteRender &renderer);

	bool IsComplete();
private:
	void init(
		std::vector<std::vector<unsigned int>> tileData,
		unsigned int levelWidth,
		unsigned int levelHeight
	);
};

