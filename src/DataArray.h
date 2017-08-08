#pragma once
#include <stdint.h>
#include <assert.h>

typedef uint32_t ID;
const uint32_t INVALID_ID = UINT32_MAX;

struct Index {
	ID id;
	uint16_t index;
	uint16_t next;
};

template<class U, unsigned short MAX_FLOW_OBJECTS = 4096>
struct DataArray {

	uint16_t numObjects;
	Index indices[MAX_FLOW_OBJECTS];
	U objects[MAX_FLOW_OBJECTS];
	uint16_t free_enqueue;
	uint16_t free_dequeue;
	
	DataArray() {
		clear();
	}

	void clear() {
		numObjects = 0;
		for (unsigned short i = 0; i < MAX_FLOW_OBJECTS; ++i) {
			indices[i].id = i;
			indices[i].next = i + 1;
		}
		free_dequeue = 0;
		free_enqueue = MAX_FLOW_OBJECTS - 1;
	}

	const bool contains(ID id) const {
		const Index& in = indices[id];
		return in.id == id && in.index != UINT16_MAX;
	}

	U& get(ID id) {
		assert(id != UINT32_MAX);
		unsigned short index = indices[id].index;
		assert(index != UINT16_MAX);
		return objects[index];
	}

	const U& get(ID id) const {
		assert(id != UINT_MAX);
		unsigned short index = indices[id].index;
		assert(index != UINT16_MAX);
		return objects[index];
	}

	ID add() {
		assert(numObjects != MAX_FLOW_OBJECTS);
		Index &in = indices[free_dequeue];
		free_dequeue = in.next;
		in.index = numObjects++;
		U& o = objects[in.index];
		o.id = in.id;
		return o.id;
	}

	ID add(const U& t) {
		assert(numObjects != MAX_FLOW_OBJECTS);
		Index &in = indices[free_dequeue];
		free_dequeue = in.next;
		in.index = numObjects++;
		U& o = objects[in.index];
		o = t;
		o.id = in.id;
		return o.id;
	}

	bool remove(ID id) {
		Index &in = indices[id];
		assert(in.index != UINT16_MAX);
		int current = in.index;
		U& o = objects[in.index];
		o = objects[--numObjects];
		indices[o.id].index = in.index;
		in.index = UINT16_MAX;
		indices[free_enqueue].next = id;
		free_enqueue = id;
		return true;
	}
};
