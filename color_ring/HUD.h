#include <diesel.h>
#include <ds_tweening.h>
#include <SpriteBatchBuffer.h>
#include <memory>

struct HUDAnimation {
	float value;
	float start;
	float end;
	float timer;
	float ttl;
	tweening::TweeningType type;
};


class HUD {

public:
	HUD();
	~HUD() {}
	void reset();
	void render(std::unique_ptr<SpriteBatchBuffer>& sprites);
	void tick(float dt);
	void incrementFilled(int value);
private:
	void drawBigNumber(std::unique_ptr<SpriteBatchBuffer>& sprites,const ds::vec2& pos, int value, int digits, float scale);
	int _score;
	int _filled;
	int _time;
	float _timer;
	HUDAnimation _hudAnimations[3];
};