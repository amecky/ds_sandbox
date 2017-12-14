#pragma once
#include <stdint.h>

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