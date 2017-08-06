#include "utils.h"
#include "..\Constants.h"
#include <string.h>

namespace font {

	ds::vec4 get_rect(char c) {
		if (c == ' ') {
			return ds::vec4(40, 0, 20, 25);
		}
		// numbers are stored at different location
		if (c >= 48 && c <= 57) {
			int idx = (int)c - 48;
			return ds::vec4(201 + idx * 30, 260, 30, 25);
		}
		// convert to lower case to upper case
		if (c > 92) {
			c -= 32;
		}
		if (c >= 65 && c <= 90) {
			ds::vec2 fd = FONT_DEF[(int)c - 65];
			return ds::vec4(200.0f + fd.x, 230.0f, fd.y, 25.0f);
		}
		return ds::vec4(40, 0, 20, 25);

	}
	void renderText(const ds::vec2& pos, const char* txt, SpriteBatchBuffer* buffer, float scale) {
		int l = strlen(txt);
		ds::vec2 p = pos;
		float lw = 0.0f;
		for (int i = 0; i < l; ++i) {
			const ds::vec4& r = get_rect(txt[i]);
			p.x += lw * 0.5f + r.z * 0.5f * scale;
			buffer->add(p, r, ds::vec2(scale,scale));
			lw = r.z * scale;
		}
	}

	ds::vec2 textSize(const char* txt) {
		int l = strlen(txt);
		ds::vec2 p(0.0f);
		for (int i = 0; i < l; ++i) {
			const ds::vec4& r = get_rect(txt[i]);
			p.x += r.z;
			if (r.w > p.y) {
				p.y = r.w;
			}
		}
		return p;
	}
}

namespace input {

	bool convertMouse2Grid(int* gridX, int* gridY) {
		ds::vec2 mousePos = ds::getMousePosition();
		float fy = (mousePos.y - static_cast<float>(STARTY - HALF_CELL_SIZE)) / static_cast<float>(CELL_SIZE);
		float fx = (mousePos.x - static_cast<float>(STARTX - HALF_CELL_SIZE)) / static_cast<float>(CELL_SIZE);
		if (fx >= 0.0f && fy >= 0.0f) {
			int mx = static_cast<int>(fx);
			int my = static_cast<int>(fy);
			if (mx < MAX_X && my < MAX_Y) {
				*gridX = mx;
				*gridY = my;
				return true;
			}
		}
		return false;
	}
}