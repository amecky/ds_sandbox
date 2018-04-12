#pragma once
#include <limits.h>
#include <assert.h>

#define INDEX_MASK 0xffff
#define NEW_OBJECT_ID_ADD 0x10000

typedef unsigned int ID;
const unsigned int INVALID_ID = UINT_MAX;

namespace ds {
	/*
	template <typename TYPE> void Construct(void* dest) {
		new ((TYPE*)dest) TYPE;
	}

	template <typename TYPE> void Destruct(void* dest) {
		((TYPE*)dest)->~TYPE();
	}
	*/
	struct Index {
		ID id;
		unsigned short index;
		unsigned short next;
	};

	template<class U, unsigned short MAX_FLOW_OBJECTS = 4096>
	struct DataArray {

		unsigned int numObjects;
		Index indices[MAX_FLOW_OBJECTS];
		U objects[MAX_FLOW_OBJECTS];
		unsigned short free_enqueue;
		unsigned short free_dequeue;
		//bool _constructor;
		//bool _destructor;
		unsigned short maxSize = MAX_FLOW_OBJECTS;
		typedef U type;
		type dataType;

		DataArray() {
			//_constructor = !__has_trivial_constructor(U);
			//_destructor = !__has_trivial_destructor(U);
			clear();
		}

		~DataArray() {
			clear();
		}

		void clear() {
			/*
			if (_destructor) {
				U* ptr = &objects[0];
				for (unsigned int i = 0; i < numObjects; ++i) {
					Destruct<U>(ptr);
					++ptr;
				}
			}
			*/
			numObjects = 0;
			for (unsigned short i = 0; i < MAX_FLOW_OBJECTS; ++i) {
				indices[i].id = i;
				indices[i].next = i + 1;
			}
			free_dequeue = 0;
			free_enqueue = MAX_FLOW_OBJECTS - 1;
		}

		const bool contains(ID id) const {
			const Index& in = indices[id & INDEX_MASK];
			return in.id == id && in.index != USHRT_MAX;
		}

		U& get(ID id) {
			assert(id != UINT_MAX);
			unsigned short index = indices[id & INDEX_MASK].index;
			assert(index != USHRT_MAX);
			return objects[index];
		}

		const U& get(ID id) const {
			assert(id != UINT_MAX);
			unsigned short index = indices[id & INDEX_MASK].index;
			assert(index != USHRT_MAX);
			return objects[index];
		}

		ID add() {
			assert(numObjects != MAX_FLOW_OBJECTS);
			Index &in = indices[free_dequeue];
			free_dequeue = in.next;
			in.index = numObjects++;
			U& o = objects[in.index];
			//if (_constructor) {
				//Construct<U>(&o);
			//}
			o.id = in.id;
			return o.id;
		}

		void remove(ID id) {
			Index &in = indices[id & INDEX_MASK];
			assert(in.index != USHRT_MAX);
			int current = in.index;
			U& o = objects[in.index];
			//if (_destructor) {
				//Destruct<U>(&o);
			//}
			o = objects[--numObjects];
			indices[o.id & INDEX_MASK].index = in.index;
			in.index = USHRT_MAX;
			indices[free_enqueue].next = id & INDEX_MASK;
			free_enqueue = id & INDEX_MASK;
		}
	};

}