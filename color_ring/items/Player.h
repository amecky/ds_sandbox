#include <diesel.h>
#include <SpriteBatchBuffer.h>

struct RenderEnvironment;

class Player {

public:
	Player(RenderEnvironment* env) : _env(env) {}
	~Player() {}
	void reset();
	void tick(float dt, float rasterizedAngle);
	float getRotation() const;
	void setColor(const ds::Color& clr);
private:
	RenderEnvironment* _env;
	SPID _id;
	float _rotation;
	SPID _dirIndicatorInnerId;
	SPID _dirIndicatorOuterId;
};