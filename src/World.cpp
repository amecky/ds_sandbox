#include "World.h"
#include "utils\tweening.h"

// ---------------------------------------------------------------
// Keyframe animation
// ---------------------------------------------------------------
void KFAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		MySprite& sp = _sprites->get(data.sprite);
		float norm = data.timer / data.ttl;
		data.animation->get(norm, &sp.animation.scale, &sp.animation.rotation, &sp.animation.position);
		data.timer += dt;
		if (data.timer > data.ttl) {
			if (data.repeat > 0) {
				--data.repeat;
				data.timer = 0.0f;
				++i;
			}
			else if (data.repeat < 0) {
				data.timer = 0.0f;
				++i;
			}
			else {
				data.animation->get(1.0f, &sp.animation.scale, &sp.animation.rotation, &sp.animation.position);
				remove(data.id);
			}
		}
		else {
			++i;
		}
	}
}

void KFAnimation::remove(ID jid) {
	_data.remove(jid);
}

// ---------------------------------------------------------------
// World
// ---------------------------------------------------------------
World::World(SpriteBatchBuffer* buffer) : _buffer(buffer) , _animations(&_spriteArray) {
}


World::~World() {
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
	for (uint16_t i = 0; i < _spriteArray.numObjects; ++i) {
		_spriteArray.objects[i].force = ds::vec2(0.0f, 0.0f);
		_spriteArray.objects[i].box.previous = _spriteArray.objects[i].position;
	}
	_animations.tick(elapsed, _queue);

	for (uint16_t i = 0; i < _spriteArray.numObjects; ++i) {
		_spriteArray.objects[i].position = _spriteArray.objects[i].basic.position + _spriteArray.objects[i].animation.position;
		_spriteArray.objects[i].scaling = _spriteArray.objects[i].basic.scale + _spriteArray.objects[i].animation.scale;
		_spriteArray.objects[i].rotation = _spriteArray.objects[i].basic.rotation + _spriteArray.objects[i].animation.rotation;
		_spriteArray.objects[i].box.position = _spriteArray.objects[i].position;
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
// find intersections
// ---------------------------------------------------------------
int World::findIntersections(Collision* collisions,int max) {
	int cnt = 0;
	for (uint16_t i = 0; i < _spriteArray.numObjects; ++i) {
		const MySprite& sp = _spriteArray.objects[i];
		const AABBox& first = sp.box;
		for (uint16_t j = 0; j < _spriteArray.numObjects; ++j) {
			if (i != j) {
				const MySprite& other = _spriteArray.objects[j];
				const AABBox& second = other.box;
				if (first.intersects(second)) {
					if (cnt < max) {
						collisions[cnt++] = { sp.position,other.position,sp.id,other.id };
					}
				}
			}
		}
	}
	return cnt;
}

void World::animate(ID id, KeyFrameAnimation* animation, float ttl, int repeat) {
	AnimationData data;
	data.animationType = ANIM_KEYFRAME;
	data.ttl = ttl;
	data.animation = animation;
	data.repeat = repeat;
	_animations.start(id, data);
}

void World::stopAll(ID id) {
	_animations.stop(id);
}

