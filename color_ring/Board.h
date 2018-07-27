#include <diesel.h>
#include <SpriteBatchBuffer.h>
#include <memory>
#include <vector>
#include "HUD.h"

struct Bullet {
	ds::vec2 pos;
	ds::vec2 velocity;
	int color;
	float rotation;
};

struct Player {
	float rotation;
};

class ColorRing;

class Board {

public:
	Board(RID textureID);
	~Board();
	void rebuildColors();
	void render();
	void tick(float dt);
	void shootBullets(float dt);
	void selectNextColor();
	void reset();
	void debug();
private:
	void drawNumber(int index, const ds::vec2& pos, float rotation);
	void drawNumber(int value, int segment);
	void moveBullets(float dt);
	ds::Color _colors[5];
	std::unique_ptr<SpriteBatchBuffer> _sprites;
	int _selectedColor;
	std::vector<Bullet> _bullets;
	float _bulletTimer;
	std::unique_ptr<ColorRing> _colorRing;
	Player _player;
	int _filled;
	HUD _hud;
};