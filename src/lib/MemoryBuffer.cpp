#include "MemoryBuffer.h"
#include <string.h>

namespace ds {

	MemoryBuffer::MemoryBuffer() : _size(0) , _capacity(0) , _data(0) {

	}

	MemoryBuffer::~MemoryBuffer() {

	}

	ID MemoryBuffer::add(void* data, size_t size) {
		if (_size + size >= _capacity) {
			int new_size = size * 8;
			if (_capacity > 0) {
				new_size = _capacity * 2 + 4 * size;
			}
			allocate(new_size);
		}
		ID id = INVALID_ID;
		if (_freeList.numObjects == 0) {
			id = _ids.add();
		}
		else {
			id = _freeList.objects[0].id;
		}
		MemoryBufferIndex& index = _ids.get(id);
		char* dest = _data + _size;
		memcpy(dest, data, size);
		index.index = _size;
		index.size = size;
		_size += size;
		return id;
	}

	void MemoryBuffer::set(ID id, void* data, size_t size) {
		if (_ids.contains(id)) {
			MemoryBufferIndex& index = _ids.get(id);
			char* dest = _data + index.index;
			if (size == index.size) {
				memcpy(dest, data, size);
			}
		}
	}

	bool MemoryBuffer::contains(ID id) {
		return true;
	}

	bool MemoryBuffer::get(ID id, void* data, size_t size) {
		MemoryBufferIndex& index = _ids.get(id);
		char* dest = _data + index.index;
		if (size == index.size) {
			memcpy(data, dest, size);
			return true;
		}
		return false;
	}

	void MemoryBuffer::allocate(uint16_t new_size) {
		if (_data == 0) {
			_data = new char[new_size];
		}
		else {
			char* tmp = new char[new_size];
			memcpy(tmp, _data, _size);
			delete[] _data;
			_data = tmp;
			_capacity = new_size;
		}
	}

}