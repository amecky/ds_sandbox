#include <diesel.h>
#include <SpriteBatchBuffer.h>

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
	void moveBullets(float dt);
	ds::Color _colors[5];
	SpriteBatchBuffer* _sprites;
	int _selectedColor;
	Bullet _bullets[64];
	int _numBullets;
	float _bulletTimer;
	ColorRing* _colorRing;
	Player _player;
	int _filled;
};