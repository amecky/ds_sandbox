#include "World.h"
#include "utils\tweening.h"

// ---------------------------------------------------------------
// Jump animation
// ---------------------------------------------------------------
void JumpAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		JumpData& data = _data.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			queue.push_event({ ANIM_JUMP,AS_FINISHED,data.sprite });
			remove(data.id);
		}
		else {
			MySprite& sp = _sprites->get(data.sprite);
			sp.position.y = data.startPos.y + tweening::interpolate(tweening::easeSinus, 0.0f, data.height, data.timer, data.ttl);
			++i;
		}
	}
}

void JumpAnimation::start(ID id, float height, float ttl) {
	ID jid = _data.add();
	JumpData& data = _data.get(jid);
	data.sprite = id;
	data.timer = 0.0f;
	data.ttl = ttl;
	const MySprite& sp = _sprites->get(id);
	data.startPos = sp.position;
	data.height = height;
}

void JumpAnimation::remove(ID jid) {
	JumpData& data = _data.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.position.y = data.startPos.y;
	_data.remove(jid);
}


// ---------------------------------------------------------------
// Rotation animation
// ---------------------------------------------------------------
void RotationAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		RotateByData& data = _data.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			remove(data.id);
			queue.push_event({ ANIM_ROTATION,AS_FINISHED,data.sprite });
		}
		else {
			MySprite& sp = _sprites->get(data.sprite);
			sp.rotation += data.step;
			++i;
		}
	}
}

void RotationAnimation::start(ID id, float angle, float ttl) {
	ID rid = _data.add();
	RotateByData& data = _data.get(rid);
	data.sprite = id;
	data.step = angle / ttl / 60.0f;
	data.timer = 0.0f;
	data.ttl = ttl;
	data.angle = angle;
	const MySprite& sp = _sprites->get(id);
	data.rotation = sp.rotation;
}

void RotationAnimation::remove(ID jid) {
	RotateByData& data = _data.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.rotation = data.rotation + data.angle;
	_data.remove(data.id);
}

// ---------------------------------------------------------------
// Squeeze animation
// ---------------------------------------------------------------
void SqueezeAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		SqueezeData& data = _data.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			remove(data.id);
			queue.push_event({ ANIM_SQUEEZE,AS_FINISHED,data.sprite });
		}
		else {
			MySprite& sp = _sprites->get(data.sprite);
			sp.scaling = tweening::interpolate(tweening::easeSinus, ds::vec2(1.0f), data.amplitude, data.timer, data.ttl);
			++i;
		}
	}
}

void SqueezeAnimation::start(ID id, const ds::vec2& amplitude, float ttl) {
	ID jid = _data.add();
	SqueezeData& data = _data.get(jid);
	data.sprite = id;
	data.timer = 0.0f;
	data.ttl = ttl;
	data.amplitude = amplitude;
}

void SqueezeAnimation::remove(ID jid) {
	SqueezeData& data = _data.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.scaling = ds::vec2(1.0f);
	_data.remove(data.id);
}

// ---------------------------------------------------------------
// Move by animation
// ---------------------------------------------------------------
void MoveByAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		MoveByData& data = _data.objects[i];
		MySprite& sp = _sprites->get(data.sprite);
		sp.position += data.velocity * dt;
		if (sp.position.x < 0.0f || sp.position.x > 1024.0f || sp.position.y < 0.0f || sp.position.y > 770.0f) {
			queue.push_event({ ANIM_MOVE_BY,AS_FINISHED,data.sprite });
		}		
		++i;
	}
}

void MoveByAnimation::start(ID id, const ds::vec2& velocity) {
	ID jid = _data.add();
	MoveByData& data = _data.get(jid);
	data.sprite = id;
	data.velocity = velocity;
}

void MoveByAnimation::remove(ID jid) {
	_data.remove(jid);
}

// ---------------------------------------------------------------
// World
// ---------------------------------------------------------------
World::World(SpriteBatchBuffer* buffer) : _buffer(buffer) {
	_animations[ANIM_JUMP]     = new JumpAnimation(&_spriteArray);
	_animations[ANIM_ROTATION] = new RotationAnimation(&_spriteArray);
	_animations[ANIM_SQUEEZE]  = new SqueezeAnimation(&_spriteArray);
	_animations[ANIM_MOVE_BY]  = new MoveByAnimation(&_spriteArray);
}


World::~World() {
	delete _animations[0];
	delete _animations[1];
	delete _animations[2];
	delete _animations[3];
}

// ---------------------------------------------------------------
// add
// ---------------------------------------------------------------
ID World::add(const ds::vec2& pos, const ds::vec4& rect) {
	if (_spriteArray.numObjects < 256) {
		return _spriteArray.add(MySprite(INVALID_ID, pos, rect));
	}
	return INVALID_ID;
}

// ---------------------------------------------------------------
// tick
// ---------------------------------------------------------------
void World::tick(float elapsed) {
	
	for (int i = 0; i < 4; ++i) {
		_animations[i]->tick(elapsed, _queue);
	}	
}

// ---------------------------------------------------------------
// render
// ---------------------------------------------------------------
void World::render() {
	for (uint16_t i = 0; i < _spriteArray.numObjects; ++i) {
		const MySprite& sp = _spriteArray.objects[i];
		_buffer->add(sp.position, sp.textureRect, sp.scaling, sp.rotation);
	}
}

// ---------------------------------------------------------------
// find intersection with mouse position
// ---------------------------------------------------------------
ID World::findIntersection(const AABBox& box) const {
	for (uint16_t i = 0; i < _spriteArray.numObjects; ++i) {
		const MySprite& sp = _spriteArray.objects[i];
		if (box.isInside(sp.position)) {
			return sp.id;
		}
	}
	return INVALID_ID;
}

// ---------------------------------------------------------------
// rotate by
// ---------------------------------------------------------------
void World::rotateBy(ID id, float angle, float ttl) {
	RotationAnimation* ra = (RotationAnimation*)_animations[ANIM_ROTATION];
	ra->start(id, angle, ttl);
}

// ---------------------------------------------------------------
// jump
// ---------------------------------------------------------------
void World::jump(ID id, float height, float ttl) {
	JumpAnimation* ja = (JumpAnimation*)_animations[ANIM_JUMP];
	ja->start(id, height, ttl);
}

// ---------------------------------------------------------------
// squeeze
// ---------------------------------------------------------------
void World::squeeze(ID id, const ds::vec2& amplitude, float ttl) {
	SqueezeAnimation* ja = (SqueezeAnimation*)_animations[ANIM_SQUEEZE];
	ja->start(id, amplitude, ttl);
}

// ---------------------------------------------------------------
// move by
// ---------------------------------------------------------------
void World::moveBy(ID id, const ds::vec2& velocity) {
	MoveByAnimation* anim = (MoveByAnimation*)_animations[ANIM_MOVE_BY];
	anim->start(id, velocity);
}

void World::stopAll(ID id) {
	for (int i = 0; i < 4; ++i) {
		_animations[i]->stop(id);
	}
}

