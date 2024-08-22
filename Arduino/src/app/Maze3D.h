#pragma once

#include "TinyApp.h"

#define MAZE_SIZE 15 // fixed

enum MazeDir { MAZE_DIR_UP, MAZE_DIR_RIGHT, MAZE_DIR_DOWN, MAZE_DIR_LEFT };

class Maze3D : public TinyApp {
public:
	virtual const char* getAppName() { return "3D Maze"; }
	virtual void setActive(bool isActive);
	virtual void update();

private:
	uint16_t _maze[MAZE_SIZE];
	int8_t _px;
	int8_t _py;
	uint8_t _pdir;
	uint8_t _goalX;
	uint8_t _goalY;

	void start();
	void draw();
	void createMaze();
	void drawMaze();
	void draw3DMaze();
	void drawGoal(const char* str);
	void goalIn();

	bool movePlayer(uint8_t dir);
	void turnPlayer(bool isRight);

	void setWall(uint16_t* maze, uint8_t x, uint8_t y, bool isSet = true);
	bool isWall(uint16_t* maze, uint8_t x, uint8_t y);
	void fillWall(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool isSet = true);
	bool isWallNearPlayer(uint8_t dist, bool isRight);
	void setDxDy(uint8_t dir, int8_t& dx, int8_t& dy);
};

