#pragma once
#include <glm/glm.hpp>

#define DISC_DEFAULT_RADIUS	(0.5f)

enum {
	DISC_STATE_NONE =-1,
	DISC_STATE_BLACK,
	DISC_STATE_WHITE,
	DISC_STATE_MAX
};

class Disc{
	glm::vec3 m_position;
	float m_radius;
	float m_color[3];
	int m_state;

public:
	Disc();
	Disc(float _x, float _y);
	Disc(glm::vec3 _position);
	void draw();
	void setPosition(float _x, float _y);
	int getState();
	void setState(int _state);
};

