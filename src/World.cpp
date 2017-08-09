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
			MySprite& sp = _sprites->get(data.sprite);
			sp.position.y = data.startPos.y;
			queue.push_event({ ANIM_JUMP,AS_FINISHED,data.sprite });
			_jumpData.remove(data.id);
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

// ---------------------------------------------------------------
// Rotation animation
// ---------------------------------------------------------------
void RotationAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _rotateByData.numObjects) {
		RotateByData& data = _rotateByData.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			MySprite& sp = _sprites->get(data.sprite);
			sp.rotation = data.rotation + data.angle;
			queue.push_event({ ANIM_ROTATION,AS_FINISHED,data.sprite });
			_rotateByData.remove(data.id);			
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

// ---------------------------------------------------------------
// Squeeze animation
// ---------------------------------------------------------------
void SqueezeAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _squeezeData.numObjects) {
		SqueezeData& data = _squeezeData.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			MySprite& sp = _sprites->get(data.sprite);
			sp.scaling = ds::vec2(1.0f);
			queue.push_event({ ANIM_SQUEEZE,AS_FINISHED,data.sprite });
			_squeezeData.remove(data.id);
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

// ---------------------------------------------------------------
// World
// ---------------------------------------------------------------
World::World(SpriteBatchBuffer* buffer) : _buffer(buffer) {
	_animations[0] = new JumpAnimation(&_spriteArray);
	_animations[1] = new RotationAnimation(&_spriteArray);
	_animations[2] = new SqueezeAnimation(&_spriteArray);
}


World::~World() {
	delete _animations[0];
	delete _animations[1];
	delete _animations[2];
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
	
	for (int i = 0; i < 3; ++i) {
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

void World::squeeze(ID id, const ds::vec2& amplitude, float ttl) {
	SqueezeAnimation* ja = (SqueezeAnimation*)_animations[ANIM_SQUEEZE];
	ja->start(id, amplitude, ttl);
}

