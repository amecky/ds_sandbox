#pragma once

class BackgroundGrid;
class Cubes;

struct QueueEntry {
	int x;
	int y;
	float timer;
	int type;
};

class EmitterQueue {

public:
	EmitterQueue(BackgroundGrid* grid, Cubes* cubes) : _grid(grid), _cubes(cubes), _numEntries(0) {}
	void tick(float dt);
	void emitt(int x, int y);
private:
	BackgroundGrid* _grid;
	Cubes* _cubes;
	QueueEntry _entries[512];
	int _numEntries;
};