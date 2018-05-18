#pragma once
#include <diesel.h>
#include "..\instancing\Billboards.h"
#include "..\lib\DataArray.h"

struct GameContext;

struct Bullet {
	ID id;
	ds::vec3 pos;
	ds::vec3 velocity;
};

class Bullets {

public:
	Bullets(GameContext* ctx, RID textureID);
	~Bullets();
	void addBullet(const ds::vec3& pos);
	void tick(float dt);
	void render(RID renderPass, const ds::matrix& viewProjection);
	uint32_t numBullets() const {
		return _bullets.numObjects;
	}
	bool collides(const ds::vec3& p, const ds::vec2& extent);
private:
	
	RID _textureID;
	GameContext* _ctx;
	Billboards* _bulletBillboards;
	ds::DataArray<Bullet> _bullets;
};