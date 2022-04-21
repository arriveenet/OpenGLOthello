#include "Disc.h"

#include <freeglut.h>

using namespace glm;

Disc::Disc()
	: m_position(0,0,0)
	, m_radius(DISC_DEFAULT_RADIUS)
	, m_color{0,0,0}
	, m_state(DISC_STATE_NONE)
{}

Disc::Disc(float _x, float _y)
	: m_position(_x,0,_y)
	, m_radius(DISC_DEFAULT_RADIUS)
	, m_color{ 0,0,0 }
	, m_state(DISC_STATE_NONE)
{}

Disc::Disc(glm::vec3 _position)
	: m_position(_position)
	, m_radius(DISC_DEFAULT_RADIUS)
	, m_color{ 0,0,0 }
	, m_state(DISC_STATE_NONE)
{}

void Disc::draw()
{
	if (m_state == DISC_STATE_NONE)
		return;

	float x, y;
	glColor3f(m_color[0], m_color[1], m_color[2]);
	glBegin(GL_POLYGON);
	for (float angle = 0; angle < 360; angle += 30) {
		float theta = glm::radians(angle);
		x = m_radius * cos(theta);
		y = -m_radius * sin(theta);
		glVertex3f(m_position.x + x, 0, m_position.z + y);
	}
	glEnd();
}

void Disc::setPosition(float _x, float _y)
{
	m_position = { _x, m_position.y, _y };
}

int Disc::getState()
{
	return m_state;
}

void Disc::setState(int _state)
{
	switch (_state) {
	case DISC_STATE_NONE:
		break;
	case DISC_STATE_BLACK:
		m_state = _state;
		m_color[0] = 0;
		m_color[1] = 0;
		m_color[2] = 0;
		break;
	case DISC_STATE_WHITE:
		m_state = _state;
		m_color[0] = 1;
		m_color[1] = 1;
		m_color[2] = 1;
		break;
	case DISC_STATE_MAX:
		break;
	default:
		break;
	}
}