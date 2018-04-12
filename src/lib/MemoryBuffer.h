#pragma once
#include <stdint.h>
#include "DataArray.h"

namespace ds {

	struct MemoryBufferIndex {
		ID id;
		uint16_t index;
		uint16_t size;
	};

	class MemoryBuffer {

	public:

		MemoryBuffer();

		~MemoryBuffer();

		ID add(void* data, size_t size);

		void set(ID id, void* data, size_t size);

		bool contains(ID id);

		bool get(ID id, void* data, size_t size);

		void remove(ID id) {}

	private:

		void allocate(uint16_t new_size);

		char* _data;
		uint16_t _size;
		uint16_t _capacity;

		DataArray<MemoryBufferIndex> _ids;
		DataArray<MemoryBufferIndex> _freeList;
	};

}