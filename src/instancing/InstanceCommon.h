#pragma once
#include <diesel.h>

enum BackgroundSide {
	BS_LEFT_SIDE = 1,
	BS_TOP_SIDE,
	BS_RIGHT_SIDE,
	BS_BOTTOM_SIDE
};

struct InstanceLightBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
	ds::vec3 lightDirection;
	float padding;
};

//struct MultipleLightsBuffer {
	//InstanceLightBuffer lights[3];
//};

struct InstanceBuffer {
	ds::matrix mvp;
	ds::matrix world;
};

struct InstanceData {
	ds::matrix worldMatrix;
	ds::Color color;
};

struct Bullet {
	ds::vec3 pos;
	ds::vec3 velocity;
	float rotation;
	float timer;
	ds::vec2 scale;
};

template<class T, unsigned short MAX_OBJECTS = 1024>
class fixed_array {

public:
	fixed_array() : _num(0) {}
	~fixed_array() {}
	void add(const T& t) {
		if (_num < MAX_OBJECTS) {
			_data[_num++] = t;
		}
	}
	const T& get(uint16_t idx) const {
		return _data[idx];
	}
	T& get(uint16_t idx) {
		return _data[idx];
	}
	void remove(uint16_t idx) {
		T& o = _data[idx];
		o = _data[--_num];
	}
	uint16_t size() const {
		return _num;
	}
	const T* ptr() const {
		return _data[0];
	}
private:
	T _data[MAX_OBJECTS];
	uint16_t _num;
};