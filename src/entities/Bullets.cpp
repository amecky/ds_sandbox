#include "Bullets.h"
#include "..\GameContext.h"

bool testBoxIntersection(const ds::vec3& p1, const ds::vec2& e1, const ds::vec3& p2, const ds::vec2& e2) {
	float top = p1.y - e1.y * 0.5f;
	float left = p1.x - e1.x * 0.5f;
	float right = p1.x + e1.x * 0.5f;
	float bottom = p1.y + e1.y * 0.5f;

	float otherTop = p2.y - e2.y * 0.5f;
	float otherLeft = p2.x - e2.x * 0.5f;
	float otherRight = p2.x + e2.x * 0.5f;
	float otherBottom = p2.y + e2.y * 0.5f;

	if (right < otherLeft || left > otherRight) return false;
	if (bottom < otherTop || top > otherBottom) return false;
	return true;
}

Bullets::Bullets(GameContext * ctx, RID textureID) : _ctx(ctx) , _textureID(textureID) {
	_bulletBillboards = new Billboards;
	_bulletBillboards->init(_textureID);
}

Bullets::~Bullets() {
	delete _bulletBillboards;
}

void Bullets::addBullet(const ds::vec3 & pos) {
	ID id = _bullets.add();
	Bullet& b = _bullets.get(id);
	b.pos = pos;
	b.pos.x += 0.5f;
	float v = _ctx->settings->bullets.velocity;
	b.velocity = ds::vec3(v, 0.0, 0.0f);
}

void Bullets::tick(float dt) {
	for (int i = 0; i < _bullets.numObjects; ++i) {
		_bullets.objects[i].pos += _bullets.objects[i].velocity * dt;
		if (_bullets.objects[i].pos.x > 12.0f) {
			_bullets.remove(_bullets.objects[i].id);
		}
	}
}

void Bullets::render(RID renderPass, const ds::matrix& viewProjection) {
	_bulletBillboards->begin();
	for (int i = 0; i < _bullets.numObjects; ++i) {
		_bulletBillboards->add(_bullets.objects[i].pos, ds::vec2(0.25f, 0.25f), ds::vec4(0, 0, 128, 128), 0.0f, ds::vec2(1.0f), ds::Color(255, 255, 0, 255));
	}
	_bulletBillboards->end();
	_bulletBillboards->render(renderPass, viewProjection);
}

bool Bullets::collides(const ds::vec3 & p, const ds::vec2 & extent) {
	for (int i = 0; i < _bullets.numObjects; ++i) {
		if (testBoxIntersection(_bullets.objects[i].pos, ds::vec2(0.1f, 0.1f), p, extent)) {
			_bullets.remove(_bullets.objects[i].id);
			return true;
		}
	}
	return false;
}
