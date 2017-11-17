#include "Cubes.h"
#include "..\Mesh.h"

void Cubes::init(RID basicGroup, RID vertexShaderId, RID pixelShaderId) {

	_scalePath.add(0.0f, 0.05f);
	_scalePath.add(0.4f, 0.25f);
	_scalePath.add(0.6f, 0.15f);
	_scalePath.add(0.8f, 0.25f);
	_scalePath.add(1.0f, 0.15f);

	_lightBuffer.ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	Mesh mesh;
	mesh.loadBin("models\\griddy.bin");
	RID cubeBuffer = mesh.assemble();

	ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, 256, sizeof(InstanceData) };
	_instanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	RID hexInstanceBuffer = ds::createInstancedBuffer(cubeBuffer, _instanceVertexBuffer);

	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	RID stateGroup = ds::StateGroupBuilder()
		.constantBuffer(lbid, pixelShaderId, 0)
		.constantBuffer(cbid, vertexShaderId, 0)
		.instancedVertexBuffer(hexInstanceBuffer)
		.build();

	ds::DrawCommand drawCmd = { mesh.getCount(), ds::DrawType::DT_INSTANCED, ds::PrimitiveTypes::TRIANGLE_LIST, 256 };

	RID groups[] = { stateGroup,basicGroup };
	_drawItem = ds::compile(drawCmd, groups, 2);

}

void Cubes::tick(float dt) {
	for (int y = 0; y < _numItems; ++y) {
		GridItem& item = _items[y];
		if (item.animationFlags == 0) {
			item.timer += dt;
			item.pos += item.velocity * ds::getElapsedSeconds();
			if (item.pos.x < -2.8f || item.pos.x > 2.8f) {
				item.velocity.x *= -1.0f;
				item.angle += ds::PI;
			}
			if (item.pos.y < -2.0f || item.pos.y > 2.0f) {
				item.velocity.y *= -1.0f;
				item.angle += ds::PI;
			}
		}
		for (int i = 0; i < item.numAnimations; ++i) {
			Animation& anim = item.animations[i];
			if (anim.timer <= anim.ttl) {
				anim.timer += dt;
				if ( ( anim.type & 1) == 1) {
					float n = anim.timer / anim.ttl;
					if (n > 1.0f) {
						n = 1.0f;
						item.animationFlags &= ~1;
					}
					item.scale = _scalePath.get(n);
				}
				if ((anim.type & 2) == 2) {
					float n = anim.timer / anim.ttl;
					if (n > 1.0f) {
						n = 1.0f;
						item.animationFlags &= ~2;
					}
					item.pos.z = 2.0f - n * 2.0f;
				}
			}
		}
	}
}

void Cubes::create(const ds::vec3& pos, int animationFlags) {
	if (_numItems < 256) {
		GridItem& item = _items[_numItems++];
		item.color = ds::Color(1.0f, 0.0f, 0.0f, 1.0f);
		item.type = 0;
		item.pos = pos;
		item.timer = 0.0f;
		item.angle = ds::random(0.0f, ds::TWO_PI);
		item.velocity = ds::vec3(1.2f * cosf(item.angle), 1.2f * sinf(item.angle), 0.0f);
		item.animationFlags = animationFlags;
		item.scale = 0.15f;
		item.numAnimations = 0;
		Animation& anim = item.animations[item.numAnimations++];
		anim.timer = 0.0f;
		anim.ttl = 0.6f;
		anim.type = 1;
		Animation& nanim = item.animations[item.numAnimations++];
		nanim.timer = 0.0f;
		nanim.ttl = 0.6f;
		nanim.type = 2;
	}
}

void Cubes::render(RID renderPass, const ds::matrix viewProjectionMatrix) {
	for (int y = 0; y < _numItems; ++y) {
		GridItem& item = _items[y];
		if (item.type == 0) {
			ds::matrix pm = ds::matTranslate(item.pos);
			ds::matrix rx = ds::matRotationX(item.timer * 4.0f);
			ds::matrix rz = ds::matRotationZ(item.angle);
			ds::matrix sm = ds::matScale(ds::vec3(item.scale, item.scale, item.scale));
			ds::matrix world = rx * rz * sm * pm;
			_instances[y] = { ds::matTranspose(world), item.color };
		}
		else {
			ds::matrix rx = ds::matRotationX(-ds::PI * 0.5f);
			ds::matrix ry = ds::matRotationY(item.angle);
			ds::matrix pm = ds::matTranslate(item.pos);
			ds::matrix world = rx * pm;
			_instances[y] = { ds::matTranspose(world), item.color };
		}

	}
	ds::mapBufferData(_instanceVertexBuffer, _instances, sizeof(InstanceData) * _numItems);
	_constantBuffer.mvp = ds::matTranspose(viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	ds::submit(renderPass, _drawItem);
}

bool collides(const ds::vec3& p1, float r1, const ds::vec3& p2, float r2) {
	ds::vec2 d = ds::vec2(p2.x,p2.y) - ds::vec2(p1.x,p1.y);
	if (sqr_length(d) < r1 * r2) {
		return true;
	}
	return false;
}

int Cubes::checkCollisions(Bullet* bullets, int num) {
	int ret = num;
	int y = 0;
	while ( y < _numItems) {
		GridItem& item = _items[y];
		ds::vec3 ip = item.pos;
		int cnt = 0;
		bool hit = false;
		while (cnt < ret) {
			const Bullet& b = bullets[cnt];
			if (collides(ip, 0.2f, b.pos, 0.1f)) {
				bullets[cnt] = bullets[ret - 1];
				--ret;
				hit = true;
			}
			else {
				++cnt;
			}
		}
		if (hit) {
			if (_numItems > 1) {
				_items[y] = _items[_numItems - 1];
			}
			--_numItems;
		}
		else {
			++y;
		}
	}
	return ret;
}