#include "World.h"
#include "utils\tweening.h"

// ---------------------------------------------------------------
// Jump animation
// ---------------------------------------------------------------
void JumpAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			queue.push_event({ ANIM_JUMP,AS_FINISHED,data.sprite });
			remove(data.id);
		}
		else {
			MySprite& sp = _sprites->get(data.sprite);
			sp.position.y = data.jump.startPos.y + tweening::interpolate(tweening::easeSinus, 0.0f, data.jump.height, data.timer, data.ttl);
			++i;
		}
	}
}

void JumpAnimation::remove(ID jid) {
	AnimationData& data = _data.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.position.y = data.jump.startPos.y;
	_data.remove(jid);
}


// ---------------------------------------------------------------
// Rotation animation
// ---------------------------------------------------------------
void RotationAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			remove(data.id);
			queue.push_event({ ANIM_ROTATION,AS_FINISHED,data.sprite });
		}
		else {
			MySprite& sp = _sprites->get(data.sprite);
			sp.rotation += data.rotateBy.step;
			++i;
		}
	}
}

void RotationAnimation::remove(ID jid) {
	AnimationData& data = _data.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.rotation = data.rotateBy.rotation + data.rotateBy.angle;
	_data.remove(data.id);
}

// ---------------------------------------------------------------
// Squeeze animation
// ---------------------------------------------------------------
void SqueezeAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			remove(data.id);
			queue.push_event({ ANIM_SQUEEZE,AS_FINISHED,data.sprite });
		}
		else {
			MySprite& sp = _sprites->get(data.sprite);
			sp.scaling = tweening::interpolate(tweening::easeSinus, ds::vec2(1.0f), data.squeeze.amplitude, data.timer, data.ttl);
			++i;
		}
	}
}

void SqueezeAnimation::remove(ID jid) {
	AnimationData& data = _data.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.scaling = ds::vec2(1.0f);
	_data.remove(data.id);
}

// ---------------------------------------------------------------
// Scale by path animation
// ---------------------------------------------------------------
void ScaleByPathAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		data.timer += dt;
		if (data.timer >= data.ttl) {
			remove(data.id);
			queue.push_event({ ANIM_SCALE_BY_PATH,AS_FINISHED,data.sprite });
		}
		else {
			MySprite& sp = _sprites->get(data.sprite);			
			sp.scaling = data.scaleByPath.path->get(data.timer / data.ttl);
			++i;
		}
	}
}

void ScaleByPathAnimation::remove(ID jid) {
	AnimationData& data = _data.get(jid);
	MySprite& sp = _sprites->get(data.sprite);
	sp.scaling = data.scaleByPath.path->get(1.0f);
	_data.remove(data.id);
}

// ---------------------------------------------------------------
// Move by animation
// ---------------------------------------------------------------
void MoveByAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		MySprite& sp = _sprites->get(data.sprite);
		sp.force += data.moveBy.velocity;
		if (sp.position.x < 0.0f || sp.position.x > _boundingRect.z || sp.position.y < 0.0f || sp.position.y > _boundingRect.w) {
			queue.push_event({ ANIM_MOVE_BY,AS_FINISHED,data.sprite });
		}		
		++i;
	}
}

void MoveByAnimation::remove(ID jid) {
	_data.remove(jid);
}

// ---------------------------------------------------------------
// Wiggle animation
// ---------------------------------------------------------------
void WiggleAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		MySprite& sp = _sprites->get(data.sprite);
		data.timer += dt;
		sp.rotation = sin(data.wiggle.frequency*data.timer*ds::TWO_PI)*data.wiggle.angle;
		++i;
	}
}

void WiggleAnimation::remove(ID jid) {
	_data.remove(jid);
}

float clamp(float v, float min, float max) {
	if (v < min) {
		return min;
	}
	if (v > max) {
		return max;
	}
	return v;
}

float calculateRotation(const ds::vec2& v) {
	ds::vec2 vn = normalize(v);
	if (vn != ds::vec2(1,0)) {
		float dt = clamp(dot(vn, ds::vec2(1,0)), -1.0f, 1.0f);
		float tmp = acos(dt);
		float cross = -1.0f * vn.y;
		if (cross > 0.0f) {
			tmp = 2.0f * ds::PI - tmp;
		}
		return tmp;
	}
	else {
		return 0.0f;
	}
}
// ---------------------------------------------------------------
// Follow animation
// ---------------------------------------------------------------
void FollowAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		if (_sprites->contains(data.follow.target)) {
			MySprite& sp = _sprites->get(data.sprite);
			const MySprite& trg = _sprites->get(data.follow.target);
			ds::vec2 p = sp.position;
			ds::vec2 targetPos = trg.position;
			ds::vec2 diff = targetPos - p;
			if (sqr_length(diff) > 100.0f) {
				ds::vec2 n = normalize(diff);
				n *= data.follow.velocity;
				//p += n;
				sp.force += n;
				//sp.position = p;
				float angle = calculateRotation(n);
				sp.rotation = angle;
			}
			++i;
		}
		else {
			remove(data.id);
		}
	}
}

void FollowAnimation::remove(ID jid) {
	_data.remove(jid);
}

// ---------------------------------------------------------------
// Keyframe animation
// ---------------------------------------------------------------
void KFAnimation::tick(float dt, EventQueue& queue) {
	int i = 0;
	while (i < _data.numObjects) {
		AnimationData& data = _data.objects[i];
		MySprite& sp = _sprites->get(data.sprite);
		float norm = data.timer / data.ttl;
		data.keyFrame.keyframe->get(norm, &sp.scaling, &sp.rotation, &sp.force);
		data.timer += dt;
		if (data.timer > data.ttl) {
			remove(data.id);
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
World::World(SpriteBatchBuffer* buffer) : _buffer(buffer) {
	_animations[ANIM_JUMP]     = new JumpAnimation(&_spriteArray);
	_animations[ANIM_ROTATION] = new RotationAnimation(&_spriteArray);
	_animations[ANIM_SQUEEZE]  = new SqueezeAnimation(&_spriteArray);
	_animations[ANIM_MOVE_BY]  = new MoveByAnimation(&_spriteArray);
	_animations[ANIM_WIGGLE] = new WiggleAnimation(&_spriteArray);
	_animations[ANIM_FOLLOW_TARGET] = new FollowAnimation(&_spriteArray);
	_animations[ANIM_SCALE_BY_PATH] = new ScaleByPathAnimation(&_spriteArray);
	_animations[ANIM_KEYFRAME] = new KFAnimation(&_spriteArray);
	_boundingRect = ds::vec4(0.0f, 0.0f, ds::getScreenWidth(), ds::getScreenHeight());
	for (int i = 0; i < MAX_ANIMATIONS; ++i) {
		_animations[i]->setBoundingRect(_boundingRect);
	}
}


World::~World() {
	for (int i = 0; i < MAX_ANIMATIONS; ++i) {
		delete _animations[i];
	}
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
	for (int i = 0; i < MAX_ANIMATIONS; ++i) {
		_animations[i]->tick(elapsed, _queue);
	}	
	for (uint16_t i = 0; i < _spriteArray.numObjects; ++i) {
		_spriteArray.objects[i].position += _spriteArray.objects[i].force * elapsed;
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

// ---------------------------------------------------------------
// rotate by
// ---------------------------------------------------------------
void World::rotateBy(ID id, float angle, float ttl) {
	BaseAnimation* ra = _animations[ANIM_ROTATION];
	AnimationData data;
	data.animationType = ANIM_ROTATION;
	data.ttl = ttl;
	data.rotateBy.step = angle / ttl / 60.0f;
	data.rotateBy.angle = angle;
	ra->start(id, data);
}

// ---------------------------------------------------------------
// jump
// ---------------------------------------------------------------
void World::jump(ID id, float height, float ttl) {
	AnimationData data;
	data.animationType = ANIM_JUMP;
	data.ttl = ttl;
	data.jump.height = height;
	const MySprite& sp = _spriteArray.get(id);
	data.jump.startPos = sp.position;
	_animations[ANIM_JUMP]->start(id, data);
}

// ---------------------------------------------------------------
// squeeze
// ---------------------------------------------------------------
void World::squeeze(ID id, const ds::vec2& amplitude, float ttl) {
	AnimationData data;
	data.animationType = ANIM_SQUEEZE;
	data.ttl = ttl;
	data.squeeze.amplitude = amplitude;
	_animations[ANIM_SQUEEZE]->start(id, data);
}

// ---------------------------------------------------------------
// move by
// ---------------------------------------------------------------
void World::moveBy(ID id, const ds::vec2& velocity) {
	AnimationData data;	
	data.animationType = ANIM_MOVE_BY;
	data.moveBy.velocity = velocity;
	_animations[ANIM_MOVE_BY]->start(id, data);
}

void World::wiggle(ID id, float angle, float frequency) {
	AnimationData data;
	data.animationType = ANIM_WIGGLE;
	data.wiggle.angle = angle;
	data.wiggle.frequency = frequency;
	_animations[ANIM_WIGGLE]->start(id, data);
}

void World::follow(ID id, ID target, float velocity) {
	AnimationData data;
	data.animationType = ANIM_FOLLOW_TARGET;
	data.follow.target = target;
	data.follow.velocity = velocity;
	_animations[ANIM_FOLLOW_TARGET]->start(id, data);
}

void World::scaleByPath(ID id, ds::Vec2Path* path, float ttl) {
	AnimationData data;
	data.animationType = ANIM_SCALE_BY_PATH;
	data.ttl = ttl;
	data.scaleByPath.path = path;
	_animations[ANIM_SCALE_BY_PATH]->start(id, data);
}

void World::animate(ID id, KeyFrameAnimation* animation) {
	AnimationData data;
	data.animationType = ANIM_KEYFRAME;
	data.ttl = animation->getTTL();
	data.keyFrame.keyframe = animation;
	_animations[ANIM_KEYFRAME]->start(id, data);
}

void World::stopAll(ID id) {
	for (int i = 0; i < MAX_ANIMATIONS; ++i) {
		_animations[i]->stop(id);
	}
}

