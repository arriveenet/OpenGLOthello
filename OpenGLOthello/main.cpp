#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/intersect.hpp>

#include "Disc.h"

#define WINDOW_WIDTH	(960)
#define WINDOW_HEIGHT	(640)

#define BOARD_WIDTH		(8)
#define BOARD_HEIGHT	(8)

using namespace glm;

Disc disc[BOARD_WIDTH][BOARD_HEIGHT];

mat4 model;
mat4 proj;
vec3 mouse3d;

vec3 center;
vec3 eye;
vec3 up;

int turn = 0;
int count[DISC_STATE_MAX];

void fontDraw(int x, int y, const char *format, ...)
{
	char buf[1024];
	va_list ap;

	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	{
		glLoadIdentity();
		va_start(ap, format);
		vsprintf_s(buf, format, ap);
		//glBitmap(0, 0, 0, 0, (int)x, (int)y, NULL);
		glRasterPos2f((GLfloat)x, (GLfloat)y);
		for (int i = 0; i < strlen(buf); i++) {
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buf[i]);
		}
		va_end(ap);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);


}

void xyzAxes(float _len)
{
	glBegin(GL_LINES);
	{
		// x line
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(_len, 0, 0);

		// y line
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, _len, 0);

		// z line
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, _len);

	}
	glEnd();
}

void drawBoard()
{
	{
		static float y = -0.01f;
		glColor3f(0, 0.5f, 0);
		glBegin(GL_QUADS);
		{

			glVertex3f(0, y, 0);
			glVertex3f(0, y, BOARD_HEIGHT);
			glVertex3f(BOARD_WIDTH, y, BOARD_HEIGHT);
			glVertex3f(BOARD_WIDTH, y, 0);
		}
		glEnd();

		// left
		glColor3ub(185, 122, 87);
		glBegin(GL_QUADS);
		{
			glVertex3f(-0.5, y, 0);
			glVertex3f(-0.5, y, BOARD_HEIGHT);
			glVertex3f(0, y, BOARD_HEIGHT);
			glVertex3f(0, y, 0);
		}
		glEnd();

		// right
		glColor3ub(185, 122, 87);
		glBegin(GL_QUADS);
		{
			glVertex3f(BOARD_WIDTH, y, 0);
			glVertex3f(BOARD_WIDTH, y, BOARD_HEIGHT);
			glVertex3f(BOARD_WIDTH + 0.5f, y, BOARD_HEIGHT);
			glVertex3f(BOARD_WIDTH + 0.5f, y, 0);
		}
		glEnd();

		// top
		glColor3ub(185, 122, 87);
		glBegin(GL_QUADS);
		{
			glVertex3f(-0.5, y, -0.5);
			glVertex3f(-0.5, y, 0);
			glVertex3f(BOARD_WIDTH + 0.5, y, 0);
			glVertex3f(BOARD_WIDTH + 0.5, y, -0.5);
		}
		glEnd();

		// buttom
		glColor3ub(185, 122, 87);
		glBegin(GL_QUADS);
		{
			glVertex3f(-0.5, y, BOARD_HEIGHT);
			glVertex3f(-0.5, y, BOARD_HEIGHT + 0.5);
			glVertex3f(BOARD_WIDTH + 0.5, y, BOARD_HEIGHT + 0.5);
			glVertex3f(BOARD_WIDTH + 0.5, y, BOARD_HEIGHT);
		}
		glEnd();
	}

	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	for (int i = 0; i <= BOARD_WIDTH; i++) {
		glVertex3d(0, 0, i);
		glVertex3d(BOARD_WIDTH, 0, i);
	}
	for (int i = 0; i <= BOARD_HEIGHT; i++) {
		glVertex3d(i, 0, 0);
		glVertex3d(i, 0, BOARD_HEIGHT);
	}
	glEnd();
}

bool checkCanPut(int _color, int _x, int _y, bool _turnOver)
{
	bool result = false;
	if ((disc[_x][_y].getState() != DISC_STATE_NONE)
		|| (_color <= DISC_STATE_NONE)
		|| (_color >= DISC_STATE_MAX))
		return false;
	
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if (dx == 0 && dy == 0)
				continue;
			int x = _x + dx;
			int y = _y + dy;
			while (1) {
				if ((x < 0) || (x >= BOARD_WIDTH) || (y < 0) || (y >= BOARD_HEIGHT)
					|| (disc[x][y].getState() == DISC_STATE_NONE)
					|| (disc[x][y].getState() == _color))
					break;
				x += dx;
				y += dy;
				if (disc[x][y].getState() == _color) {
					result = true;
					if (_turnOver) {
						int nx = _x, ny = _y;
						while (1) {
							if ((nx < 0) || (nx >= BOARD_WIDTH) || (ny < 0) || (ny >= BOARD_HEIGHT))
								break;
							disc[nx][ny].setState(_color);
							nx += dx;
							ny += dy;
							if ((x == nx) && (y == ny))
								break;
						}
					}
				}
			}
		}

	return result;
}

bool checkCanPutAll(int _color) {
	bool result = false;
	for (int y = 0; y < BOARD_HEIGHT; y++)
		for (int x = 0; x < BOARD_WIDTH; x++)
			if (checkCanPut(_color, x, y, false)) {
				result = true;
				glColor3f(0, 0.6f, 0);
				glBegin(GL_QUADS);
				{
					static float x2 = 0.05f;
					static float y2 = -0.001f;
					glVertex3f((float)x + x2, y2, (float)y + x2);
					glVertex3f((float)x + x2, y2, y + 1.f - x2);
					glVertex3f(x + 1.f - x2, y2, y + 1.f - x2);
					glVertex3f(x + 1.f - x2, y2, y + x2);
				}
				glEnd();
			}

	return result;
}

void display(void)
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	proj = perspective(
		float(60 * M_PI /180),// T fovy
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,// T aspect
		0.1f,// T zNear
		100.0f);// T zFar
	glLoadMatrixf((const GLfloat * )&proj);

	glMatrixMode(GL_MODELVIEW);
	center = { BOARD_WIDTH / 2.f, 0, BOARD_WIDTH / 2.f + 1 };
	eye = center + vec3(0, 8, 4);
	up = { 0,1,0 };
	/*
	{
		static float f;
		f += float(M_PI * 2 / (60 * 60));
		eye = center + glm::vec3(-cosf(f), 0.75f, sinf(f)) * 10.0f;
	}*/

	model = lookAt(
		eye,// vec<3, T, Q> const& eye
		center,// vec<3, T, Q> const& center
		up);// vec<3, T, Q> const& up
	glLoadMatrixf((const GLfloat*)&model); // const GLfloat *m

	glPushMatrix();

	drawBoard();

	for (int x = 0; x < BOARD_WIDTH; x++)
		for (int y = 0; y < BOARD_HEIGHT; y++)
			disc[x][y].draw();

	checkCanPutAll(turn);
	xyzAxes(1.f);
	glPopMatrix();

	glColor3ub(0xff, 0xff, 0xff);
	const char* color[] = {
		"BLACK",
		"WHITE"
	};
	fontDraw(10,10, "turn: %s",color[turn]);
	fontDraw(10,20, "black: %d", count[DISC_STATE_BLACK]);
	fontDraw(10, 30, "white: %d", count[DISC_STATE_WHITE]);

	glutSwapBuffers();
}

void idle(void)
{
	if(!checkCanPutAll(turn))
		turn ^= 1;

	count[0] = 0;
	count[1] = 0;
	for (int y = 0; y < BOARD_HEIGHT; y++)
		for (int x = 0; x < BOARD_WIDTH; x++)
			if (disc[y][x].getState() != DISC_STATE_NONE)
				count[disc[y][x].getState()]++;

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouse3d = unProject(
			vec3(x,WINDOW_HEIGHT-y,0),// vec<3, T, Q> const& win
			model,// mat<4, 4, T, Q> const& model
			proj,// mat<4, 4, T, Q> const& proj
			vec4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));// vec<4, U, Q> const& viewport
		//printf("%d %d -> %f %f %f\n", x, y, mouse3d.x, mouse3d.y, mouse3d.z);

		vec3 dir = mouse3d - eye;
		float intersectionDistance;
		if (intersectRayPlane(
			eye,// genType const& orig
			dir,// genType const& dir
			{ 0,0,0 },// genType const& planeOrig
			{ 0,1,0 },// genType const& planeNormal
			intersectionDistance))// typename genType::value_type & intersectionDistance
			mouse3d = eye + dir * intersectionDistance;
		else
			mouse3d = { 0,0,0 };

		int x = (int)mouse3d.x;
		int y = (int)mouse3d.z;
		
		if(checkCanPut(turn, x, y, true))
			turn ^= 1;
	
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowPosition(640, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Othello");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		60.0,	// GLdouble fovy
		(GLdouble)WINDOW_WIDTH / WINDOW_HEIGHT,	// GLdouble aspect
		0.1,	// GLdouble zNear
		0.0);// GLdouble zFar

	for(int x = 0; x < BOARD_WIDTH; x++)
		for (int y = 0; y < BOARD_HEIGHT; y++) 
			disc[x][y].setPosition(x+DISC_DEFAULT_RADIUS, y+DISC_DEFAULT_RADIUS);
	disc[3][3].setState(DISC_STATE_WHITE);
	disc[3][4].setState(DISC_STATE_BLACK);
	disc[4][4].setState(DISC_STATE_WHITE);
	disc[4][3].setState(DISC_STATE_BLACK);

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);

	glutMainLoop();
	return 0;
}