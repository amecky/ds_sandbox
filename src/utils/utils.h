#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>

namespace font {

	ds::vec4 get_rect(char c);

	void renderText(const ds::vec2& pos, const char* txt, SpriteBatchBuffer* buffer, float scale = 1.0f);

	ds::vec2 textSize(const char* txt);
}

namespace input {

	bool convertMouse2Grid(int* gridX, int* gridY);
}