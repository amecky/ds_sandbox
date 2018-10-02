#include <diesel.h>
#include <ds_tweening.h>
#include <SpriteBatchBuffer.h>
#include <memory>

struct RenderEnvironment;

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
	HUD(RenderEnvironment* env);
	~HUD() {}
	void reset();
	void tick(float dt);
	void incrementFilled(int value);
	void incrementScore(int value);
private:
	RenderEnvironment* _env;
	void addNumbers(const ds::vec2& pos, SPID* ids, int digits);
	void updateNumbers(const ds::vec2& pos, SPID* ids, int digits, int value);
	int _score;
	int _filled;
	int _time;
	float _timer;
	HUDAnimation _hudAnimations[3];
	SPID _timerIds[2];
	SPID _scoreIds[6];
	SPID _counterIds[2];
};