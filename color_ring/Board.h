#include <diesel.h>
#include <ds_tweening.h>
#include <memory>
#include <vector>
#include "HUD.h"

struct Bullet {
	ds::vec2 velocity;
	int color;
	Sprite sprite;
};

struct DirectionIndicator {
	Sprite inner;
	Sprite outer;
};

struct ColorSelection {
	Sprite boxes[4];
	Sprite borders[4];
};

struct Player {
	Sprite sprite;
	float rotation;
};

struct AnimatedLetter {
	ds::vec2 start;
	ds::vec2 end;
	ds::vec2 pos;
	float timer;
	float ttl;
	int texureIndex;
	tweening::TweeningType type;
};

class ColorRing;
struct RenderEnvironment;

class Board {

public:
	Board(RenderEnvironment* env);
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
	void drawSegmentTimer(int value, int segment);
	void moveBullets(float dt);
	RenderEnvironment* _env;
	ds::Color _colors[5];
	int _selectedColor;
	std::vector<Bullet> _bullets;
	float _bulletTimer;
	std::unique_ptr<ColorRing> _colorRing;
	Player _player;
	HUD _hud;
	bool _pressed;
	std::vector<AnimatedLetter> _animatedLetters;

	DirectionIndicator _directionIndicator;
	ColorSelection _colorSelection;
	
};