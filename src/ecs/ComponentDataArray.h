#pragma once
#include <stdint.h>
#include <typeinfo.h>
#include <assert.h>

typedef uint32_t ID;
const uint32_t NO_ID = UINT32_MAX;

#define INDEX_MASK 0xffff
#define NEW_OBJECT_ID_ADD 0x10000

const int MAX_BLOCKS = 16;

namespace ds {

	struct Index {
		ID id;
		uint16_t index;
		uint16_t next;
	};

	struct ChannelArrayDefinition {
		const std::type_info* typeInfo;
		size_t size;
	};

	struct ChannelArray {

		char* data;
		uint16_t size;
		uint16_t capacity;
		uint16_t total_capacity;
		uint16_t _sizes[MAX_BLOCKS];
		const std::type_info* _typeInfo[MAX_BLOCKS];
		uint16_t _indices[MAX_BLOCKS];
		uint16_t _num_blocks;
		Index* _data_indices;
		uint16_t _free_enqueue;
		uint16_t _free_dequeue;

		ChannelArray();

		~ChannelArray();

		void init(ChannelArrayDefinition* sizes, uint16_t num);

		ID add();

		template<class T>
		void set(ID id, uint16_t channel, const T& t) {
			T* p = (T*)get_ptr(channel);
			assert(id != UINT_MAX);
			uint16_t index = _data_indices[id & INDEX_MASK].index;
			assert(index != USHRT_MAX);
			p[index] = t;
		}

		const bool contains(ID id) const {
			const Index& in = _data_indices[id & INDEX_MASK];
			return in.id == id && in.index != UINT16_MAX;
		}

		void remove(ID id);

		template<class T>
		const T& get(ID id, uint16_t channel) const {
			const T* p = get_ptr(channel);
			assert(id != UINT_MAX);
			uint16_t index = _data_indices[id & INDEX_MASK].index;
			return p[index];
		}

		template<class T>
		T& get(ID id, int channel) {
			T* p = (T*)get_ptr(channel);
			assert(id != UINT_MAX);
			uint16_t index = _data_indices[id & INDEX_MASK].index;
			return p[index];
		}

		template<class T>
		T* get_ptr(uint16_t channel) {
			assert(typeid(T) == *_typeInfo[channel]);
			return (T*)(data + _indices[channel]);
		}

		void* get_ptr(uint16_t channel);

		bool resize(uint16_t new_size);

	};

}
