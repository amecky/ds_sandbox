#include "World.h"
#include "utils\tweening.h"

// ---------------------------------------------------------------
// Jump animation
// ---------------------------------------------------------------
void JumpAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _jumpData.numObjects) {
		JumpData& data = _jumpData.objects[i];
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
	ID jid = _jumpData.add();
	JumpData& data = _jumpData.get(jid);
	data.sprite = id;
	data.timer = 0.0f;
	data.ttl = ttl;
	const MySprite& sp = _sprites->get(id);
	data.startPos = sp.position;
	data.height = height;
}

void JumpAnimation::stop(ID id) {
	ID jid = findBySpriteID(id);
	if (jid != INVALID_ID) {
		remove(jid);
	}
}

void JumpAnimation::remove(ID jid) {
	JumpData& data = _jumpData.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.position.y = data.startPos.y;
	_jumpData.remove(jid);
}

ID JumpAnimation::findBySpriteID(ID id) {
	int i = 0;
	while (i < _jumpData.numObjects) {
		if (_jumpData.objects[i].sprite == id) {
			return _jumpData.objects[i].id;
		}
		++i;
	}
	return INVALID_ID;
}
// ---------------------------------------------------------------
// Rotation animation
// ---------------------------------------------------------------
void RotationAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _rotateByData.numObjects) {
		RotateByData& data = _rotateByData.objects[i];
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
	ID rid = _rotateByData.add();
	RotateByData& data = _rotateByData.get(rid);
	data.sprite = id;
	data.step = angle / ttl / 60.0f;
	data.timer = 0.0f;
	data.ttl = ttl;
	data.angle = angle;
	const MySprite& sp = _sprites->get(id);
	data.rotation = sp.rotation;
}

void RotationAnimation::stop(ID id) {
	ID jid = findBySpriteID(id);
	if (jid != INVALID_ID) {
		remove(jid);
	}
}

void RotationAnimation::remove(ID jid) {
	RotateByData& data = _rotateByData.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.rotation = data.rotation + data.angle;
	_rotateByData.remove(data.id);
}

ID RotationAnimation::findBySpriteID(ID id) {
	int i = 0;
	while (i < _rotateByData.numObjects) {
		if (_rotateByData.objects[i].sprite == id) {
			return _rotateByData.objects[i].id;
		}
		++i;
	}
	return INVALID_ID;
}

// ---------------------------------------------------------------
// Squeeze animation
// ---------------------------------------------------------------
void SqueezeAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _squeezeData.numObjects) {
		SqueezeData& data = _squeezeData.objects[i];
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
	ID jid = _squeezeData.add();
	SqueezeData& data = _squeezeData.get(jid);
	data.sprite = id;
	data.timer = 0.0f;
	data.ttl = ttl;
	data.amplitude = amplitude;
}

void SqueezeAnimation::stop(ID id) {
	ID jid = findBySpriteID(id);
	if (jid != INVALID_ID) {
		remove(jid);
	}
}

void SqueezeAnimation::remove(ID jid) {
	SqueezeData& data = _squeezeData.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.scaling = ds::vec2(1.0f);
	_squeezeData.remove(data.id);
}

ID SqueezeAnimation::findBySpriteID(ID id) {
	int i = 0;
	while (i < _squeezeData.numObjects) {
		if (_squeezeData.objects[i].sprite == id) {
			return _squeezeData.objects[i].id;
		}
		++i;
	}
	return INVALID_ID;
}

// ---------------------------------------------------------------
// Move by animation
// ---------------------------------------------------------------
void MoveByAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _moveByData.numObjects) {
		MoveByData& data = _moveByData.objects[i];
		MySprite& sp = _sprites->get(data.sprite);
		sp.position += data.velocity * dt;
		if (sp.position.x < 0.0f || sp.position.x > 1024.0f || sp.position.y < 0.0f || sp.position.y > 770.0f) {
			queue.push_event({ ANIM_MOVE_BY,AS_FINISHED,data.sprite });
			//_moveByData.remove(data.id);
		}		
		++i;
	}
}

void MoveByAnimation::start(ID id, const ds::vec2& velocity) {
	ID jid = _moveByData.add();
	MoveByData& data = _moveByData.get(jid);
	data.sprite = id;
	data.velocity = velocity;
}

void MoveByAnimation::stop(ID id) {
	ID jid = findBySpriteID(id);
	if (jid != INVALID_ID) {
		remove(jid);
	}
}

void MoveByAnimation::remove(ID jid) {
	_moveByData.remove(jid);
}

ID MoveByAnimation::findBySpriteID(ID id) {
	int i = 0;
	while (i < _moveByData.numObjects) {
		if (_moveByData.objects[i].sprite == id) {
			return _moveByData.objects[i].id;
		}
		++i;
	}
	return INVALID_ID;
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

