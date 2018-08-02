#include <diesel.h>
#include <SpriteBatchBuffer.h>
#include <memory>
#include <vector>
#include "particles\ParticleManager.h"

struct Bullet {
	ds::vec2 pos;
	ds::vec2 velocity;
	int color;
	float rotation;
};

struct Player {
	float rotation;
};

struct HUD {
	int score;
	int filled;
	int time;
	float timer;
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
	void rebound(const ds::vec2& p, float angle, const ds::Color& clr);
	void showFilled(int segment, const ds::Color& clr);
	void drawHUD();
	void drawBigNumber(const ds::vec2& pos, int value, int digits);
	void drawNumber(int index, const ds::vec2& pos, float rotation);
	void drawNumber(int value, int segment);
	void moveBullets(float dt);
	ds::Camera _camera;
	std::unique_ptr<ParticleManager> _particles;
	ds::Color _colors[5];
	std::unique_ptr<SpriteBatchBuffer> _sprites;
	int _selectedColor;
	std::vector<Bullet> _bullets;
	float _bulletTimer;
	std::unique_ptr<ColorRing> _colorRing;
	Player _player;
	HUD _hud;
	bool _pressed;
};