#pragma once
#include <diesel.h>
#include <vector>

class Obstacles {

public:
	Obstacles(RID textureId);
	~Obstacles();
	void add(const ds::vec2& pos);
	void render();
private:
	RID _textureId;
	std::vector<ds::vec2> _positions;
};

