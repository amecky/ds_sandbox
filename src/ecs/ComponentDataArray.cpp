#include "ComponentDataArray.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

namespace ds {

	ChannelArray::ChannelArray() : size(0), capacity(0), data(0), total_capacity(0), _data_indices(0), _free_dequeue(0), _free_enqueue(0) {
	}


	ChannelArray::~ChannelArray() {
		if (_data_indices != 0) {
			delete[] _data_indices;
		}
		if (data != 0) {
			delete[] data;
		}
	}

	void ChannelArray::init(ChannelArrayDefinition* defs, uint16_t num) {
		assert(num < MAX_BLOCKS);
		for (int i = 0; i < num; ++i) {
			_sizes[i] = defs[i].size;
			_typeInfo[i] = defs[i].typeInfo;
		}
		_num_blocks = num;
	}

	ID ChannelArray::add() {
		if (size + 1 > capacity) {
			resize(size * 2 + 8);
		}
		Index &in = _data_indices[_free_dequeue];
		_free_dequeue = in.next;
		in.index = size++;
		return in.id;
	}

	void ChannelArray::clear() {

	}

	bool ChannelArray::resize(uint16_t new_size) {
		if (new_size > capacity) {
			int total = 0;
			for (int i = 0; i < _num_blocks; ++i) {
				total += _sizes[i];
			}
			if (_data_indices == 0) {
				_data_indices = new Index[new_size];
				for (unsigned short i = 0; i < new_size; ++i) {
					_data_indices[i].id = i;
					_data_indices[i].next = i + 1;
				}
				_free_dequeue = 0;
				_free_enqueue = new_size - 1;
			}
			else {
				Index* tmp = new Index[new_size];
				memcpy(tmp, _data_indices, size * sizeof(Index));
				for (unsigned short i = size; i < new_size; ++i) {
					tmp[i].id = i;
					tmp[i].next = i + 1;
				}
				delete[] _data_indices;
				_data_indices = tmp;
				_free_enqueue = new_size - 1;
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
				_indices[i] = _indices[i - 1] + _sizes[i] * capacity;
			}
			data = t;
			return true;
		}
		return false;
	}

	void* ChannelArray::get_ptr(uint16_t index) {
		assert(index < _num_blocks);
		return data + _indices[index];
	}

	const bool ChannelArray::contains(ID id) const {
		const Index& in = _data_indices[id & INDEX_MASK];
		return in.id == id && in.index != UINT16_MAX;
	}

	void ChannelArray::remove(ID id) {
		Index &in = _data_indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		int current = in.index;
		int next = (size - 1);		
		int src = next * _sizes[0];
		int dest = current * _sizes[0];
		for (int i = 0; i < _num_blocks; ++i) {
			memcpy(data + dest, data + src, _sizes[i]);
			src += capacity * _sizes[i];
			dest += capacity * _sizes[i];
		}
		--size;
		_data_indices[in.id & INDEX_MASK].index = in.index;
		in.index = USHRT_MAX;
		_data_indices[_free_enqueue].next = id & INDEX_MASK;
		_free_enqueue = id & INDEX_MASK;
	}

}
