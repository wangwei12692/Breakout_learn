#pragma once
#include <glm/glm.hpp>
#include "game_object.h"
#include "texture.h"

class BallObject :public GameObject {
public:
	float Radius;
	bool Stuck;
	bool Sticky,PassThrouth;

	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2 Move(float dt, unsigned int window_width);
	void Reset(glm::vec2 position, glm::vec2 velocity);
};