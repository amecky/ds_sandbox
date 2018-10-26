#include "Obstacles.h"
#include <ds_sprites.h>

const ds::vec4 OBSTACLE_RECT = ds::vec4();

Obstacles::Obstacles(RID textureId) : _textureId(textureId) {
}

Obstacles::~Obstacles() {
}

void Obstacles::add(const ds::vec2 & pos) {
	_positions.push_back(pos);
}

void Obstacles::render() {
	for (int i = 0; i < _positions.size(); ++i) {
		sprites::draw(_textureId, _positions[i], OBSTACLE_RECT);
	}
}
