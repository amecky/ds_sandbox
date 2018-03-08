#pragma once
#include <ds_imgui.h>

const static int START_X = 200;
const static int START_Y = 62;
const static int GRID_SIZE_X = 20;
const static int GRID_SIZE_Y = 12;

inline bool operator ==(const p2i& first, const p2i& other) {
	return first.x == other.x && first.y == other.y;
}

inline bool operator !=(const p2i& first, const p2i& other) {
	return !(first == other);	
}

inline p2i operator +(const p2i& lhs, const p2i& rhs) {
	return p2i(lhs.x + rhs.x, lhs.y + rhs.y);
}

// ---------------------------------------------------------------
// convert screen coordinates to grid position if possible
// ---------------------------------------------------------------
inline bool convert(int screenX, int screenY, int startX, int startY, p2i* ret) {
	ret->x = (screenX - startX);
	ret->y = (screenY - startY);
	return true;
	/*
	if (screenX >= (startX - 23) && screenY >= (startY - 23)) {
		ret->x = (screenX - startX + 23) / 46;
		ret->y = (screenY - startY + 23) / 46;
		return true;
	}
	return false;
	*/
}

inline bool convert(int screenX, int screenY, p2i* ret) {
	int sx = START_X - 23;
	int sy = START_Y - 23;
	if (screenX >= sx && screenY >= sy) {
		int rx = (screenX - START_X + 23) / 46;
		int ry = (screenY - START_Y + 23) / 46;
		if (rx >= 0 && rx < GRID_SIZE_X && ry >= 0 && ry < GRID_SIZE_Y) {
			ret->x = rx;
			ret->y = ry;
			return true;
		}
	}
	return false;
}

const static ds::vec4 GRID_TEXTURES[] = {
	ds::vec4( 46,   0, 46, 46),
	ds::vec4(  0,   0, 46, 46),
	ds::vec4(138,   0, 46, 46),
	ds::vec4(184,   0, 46, 46),
	ds::vec4(  0,   0, 46, 46),
	ds::vec4( 46,   0, 46, 46),
	ds::vec4(  0, 184, 46, 46),
	ds::vec4( 46, 184, 46, 46),
	ds::vec4( 92, 184, 46, 46),
	ds::vec4(138, 184, 46, 46),
	ds::vec4(184, 184, 46, 46),
	ds::vec4(230, 184, 46, 46),
	ds::vec4(276, 184, 46, 46),
	ds::vec4(322, 184, 46, 46),
	ds::vec4(368, 184, 46, 46),
	ds::vec4(414, 184, 46, 46),
	ds::vec4(460, 184, 46, 46),
	ds::vec4(  0, 230, 46, 46),
	ds::vec4( 46, 230, 46, 46)
};



const p2i INVALID_POINT = p2i(1, 1);

struct Grid {
	
	int* items;
	int width;
	int height;
	p2i start;
	p2i end;
	
	Grid() : items(0), width(0), height(0), start(-1, -1), end(-1, -1) {}
	
	Grid(int w, int h) : width(w), height(h) {
		int total = width * height;
		items = new int[total];
		for (int i = 0; i < total; ++i) {
			items[i] = 0;
			
		}
		
	}
	
	~Grid() {
		if (items != 0) {
			delete[] items;
			
		}
		
	}

	void clear(int v) {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				set(x, y, v);
			}
		}
	}
	
	void setStart(int x, int y) {
		setStart(p2i(x, y));
		
	}
	
	void setStart(p2i s) {
		if (isValid(s) && isAvailable(s)) {
			set(s, 2);
			start = s;
		}
	}

	p2i getStart() const {
		return start;
	}
	
	void setEnd(int x, int y) {
		setEnd(p2i(x, y));
	}
	
	void setEnd(p2i e) {
		if (isValid(e) && isAvailable(e)) {
			set(e, 3);
			end = e;
		}
	}
	
	p2i getEnd() const {
		return end;
	}

	bool isValid(p2i p) const {
		return isValid(p.x, p.y);
	}
	
	void set(p2i p, int v) {
		set(p.x, p.y, v);
	}
	
	void set(int x, int y, int v) {
		if (isValid(x, y)) {
			int idx = x + y * width;
			items[idx] = v;
		}
	}

	int get(p2i p) const {
		return items[p.x + p.y * width];
	}

	int get(int x, int y) const {
		return items[x + y * width];
	}
	
	bool isValid(int x, int y) const {
		if (x < 0 || y < 0 || x >= width || y >= height) {
			return false;
		}
		return true;
	}
	
	bool isAvailable(p2i p) const {
		return isAvailable(p.x, p.y);
		
	}
	
	bool isAvailable(int x, int y) const {
		if (isValid(x, y)) {
			int idx = x + y * width;
			return items[idx] != 1 && items[idx] < 4;
		}
		return false;
		
	}
	
	int getIndex(p2i p) const {
		return getIndex(p.x, p.y);
	}
	
	int getIndex(int x, int y) const {
		return x + y * width;
	}

	bool load(const char* name) {
		char fileName[128];
		sprintf_s(fileName, "%s.lvl", name);
		int current = 0;
		FILE* fp = fopen(fileName, "rb");
		if (fp) {
			for (int y = 0; y < GRID_SIZE_Y; ++y) {
				for (int x = 0; x < GRID_SIZE_X; ++x) {
					fread(&current, sizeof(int), 1, fp);
					set(x, y, current);
					if (current == 2) {
						setStart(x, y);
					}
					if (current == 3) {
						setEnd(x, y);
					}
				}
			}
			fclose(fp);
			return true;
		}
		return false;
	}

	bool save(const char* name) {
		char fileName[128];
		sprintf_s(fileName, "%s.lvl", name);
		FILE* fp = fopen(fileName, "wb");
		if (fp) {
			for (int y = 0; y < GRID_SIZE_Y; ++y) {
				for (int x = 0; x < GRID_SIZE_X; ++x) {
					int current = get(x, y);
					fwrite(&current, sizeof(int), 1, fp);
				}
			}
			fclose(fp);
			return true;
		}
		return false;
	}
	
};


