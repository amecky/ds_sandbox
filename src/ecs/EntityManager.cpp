#include "EntityManager.h"



EntityManager::EntityManager() {
	for (int i = 0; i < 4096; ++i) {
		_indices[i].id = i;
		_indices[i].index = EMPTY_ID;
		_indices[i].num = 0;
	}
	_offset = 0;
}


EntityManager::~EntityManager() {
	for (size_t i = 0; i < _streams.size(); ++i) {
		delete _streams[i];
	}
}

void EntityManager::debug() {
	printf("----------- DEBUG ---------------\n");
	for (int i = 0; i < 4096; ++i) {
		if (_indices[i].index != EMPTY_ID) {
			const EntityIndex& idx = _indices[i];
			printf("id: %d num: %d index: %d archetype: %d\n", idx.id, idx.num, idx.index, idx.archeType);
			for (int j = 0; j < idx.num; ++j) {
				const ComponentMapping& mapping = _mappings[idx.index + j];
				if (mapping.component_id != EMPTY_ID) {
					printf("  %d  id: %d stream: %d channel: %d\n", j, mapping.component_id, mapping.stream_index, mapping.channel_index);
				}
			}
		}
	}
	for (size_t i = 0; i < _streams.size(); ++i) {
		printf("stream %d -> size %d\n", i, _streams[i]->size);
	}
}


