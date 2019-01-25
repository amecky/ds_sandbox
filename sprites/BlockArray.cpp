#include "BlockArray.h"
#include <string.h>
#include <assert.h>

BlockArray::BlockArray() : size(0), capacity(0), data(0) , total_capacity(0) {
}


BlockArray::~BlockArray() {
	if (data != 0) {
		delete[] data;
	}
}

void BlockArray::init(int* sizes, int num) {
	assert(num < MAX_BLOCKS);
	int total = 0;
	for (int i = 0; i < num; ++i) {
		_sizes[i] = sizes[i];
	}
	_num_blocks = num;
}

bool BlockArray::resize(int new_size) {
	if (new_size > capacity) {
		new_size = capacity * 2 + new_size;
		int total = 0;
		for (int i = 0; i < _num_blocks; ++i) {
			total += _sizes[i];
		}
		
		int sz = new_size * total;
		char* t = new char[sz];
		if (data != 0) {
			int offset = 0;
			int old_offset = 0;
			for (int i = 0; i < _num_blocks; ++i) {
				memcpy(t + offset, data + old_offset, size * _sizes[i]);
				offset += new_size * _sizes[i];
				old_offset += capacity * _sizes[i];
			}
			delete[] data;
		}
		capacity = new_size;
		_indices[0] = 0;
		for (int i = 1; i < _num_blocks; ++i) {
			_indices[i] = _indices[i - 1] + _sizes[i- 1] * capacity;
		}
		data = t;
		return true;
	}
	return false;
}

void* BlockArray::get_ptr(int index) {
	return data + _indices[index];
}

void BlockArray::remove(int index) {
	int last = size - 1;
	for (int i = 0; i < _num_blocks; ++i) {
		char* dest = data + _indices[i] + index * _sizes[i];
		char* src = data + _indices[i] + last * _sizes[i];
		memcpy(dest, src, _sizes[i]);
	}
	--size;
}
