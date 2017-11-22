#pragma once
#include "..\utils\EventStream.h"

struct QueueEntry {
	int x;
	int y;
	float timer;
	int type;
};

class EmitterQueue {

public:
	EmitterQueue() : _numEntries(0) {}
	void tick(float dt, ds::EventStream* events);
	void emitt(int x, int y, ds::EventStream* events);
private:
	QueueEntry _entries[512];
	int _numEntries;
};