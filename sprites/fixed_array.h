#pragma once
#include <assert.h>

template<class T>
struct FixedArray {

	FixedArray(size_t capacity) : _num(0) , _capacity(capacity)  {
		_data = new T[_capacity];
	}
	~FixedArray() {
		delete[] _data;
	}

	T& create() {
		if (num < _capacity) {
			return data[num++];
		}
	}
	void add(const T& t) {
		assert(i < _capacity);
		data[num++] = t;		
	}
	size_t capacity() {
		return _capacity;
	}
	T operator[] (size_t i) const { 
		assert(i < _capacity);
		return _data[i]; 
	};
	T& operator[] (size_t i) { 
		assert(i < _capacity);
		return _data[i]; 
	};
	const T* operator() () const {
		return &_data[0];
	}
	void remove(size_t idx) {
		if (_num > 0) {
			_data[idx] = _data[_num - 1];
			--_num;
		}
	}
	void fill(const T& t) {
		for (size_t i = 0; i < _capacity; ++i) {
			_data[i] = t;
		}
		_num = _capacity;
	}

	T* _data;
	size_t _num;
	size_t _capacity;
};
