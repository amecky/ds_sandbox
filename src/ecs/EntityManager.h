#pragma once
#include "ComponentDataArray.h"
#include <vector>

// ---------------------------------------------------------------
// Components
// ---------------------------------------------------------------
struct BaseComponent {
	ID id;

	BaseComponent() {
		id = NO_ID;
	}
};

struct Position : BaseComponent {

	float x, y, z;

	Position(float _x, float _y, float _z) : BaseComponent() , x(_x), y(_y), z(_z) {}
};

struct Velocity : BaseComponent {

	float x, y, z;

	Velocity(float _x, float _y, float _z) : BaseComponent(), x(_x), y(_y), z(_z) {}
};

class MoveSystem {

public:
	Position* positions;
	Velocity* velocities;
	
	void tick(float dt, int num) {

	}
};

// ---------------------------------------------------------------
// Archetype
// ---------------------------------------------------------------
struct ArcheType {
	const std::type_info* info;
};

// ---------------------------------------------------------------
// Archetype definition
// ---------------------------------------------------------------
struct ArcheTypeDefinition {
	std::vector<const std::type_info*> infos;
	int num;
};

const static uint16_t EMPTY_ID = UINT16_MAX;

// ---------------------------------------------------------------
// EntityManager
// ---------------------------------------------------------------
class EntityManager {

	struct EntityIndex {
		ID id;
		uint16_t index;
		uint16_t num;
		uint16_t archeType;
	};

public:
	EntityManager();
	~EntityManager();

	template<class U>
	void addComponent() {
		ds::ChannelArrayDefinition defs[] = { {&typeid(U), sizeof(U)} };
		int sidx = createStream(defs, 1);
	}


	template<class U>
	void setComponentData(ID entity, const U& data) {
		EntityIndex& index = _indices[entity];
		assert(index.index != EMPTY_ID);
		int sidx = 0;
		int cidx = 0;
		if (findMatchingStream(&typeid(U), &sidx, &cidx)) {
			int compIndex = -1;
			const ArcheTypeDefinition& def = _archeTypes[index.archeType];
			for (size_t k = 0; k < def.infos.size(); ++k) {
				if (compIndex == -1 && typeid(U) == *(def.infos[k])) {
					compIndex = k;
				}
			}
			if (compIndex != -1) {
				ID id = _streams[sidx]->add();
				_streams[sidx]->set<U>(id, cidx, data);
				int offset = index.index + compIndex;
				_mappings[offset] = id;
			}
		}
	}

	ID createEntity(ID archeTypeID) {
		// FIXME: assert that we have the archetype
		const ArcheTypeDefinition& def = _archeTypes[archeTypeID];
		int fidx = findFreeId();
		if (fidx != -1) {
			EntityIndex& idx = _indices[fidx];
			idx.index = _offset;
			idx.num = def.num;
			idx.archeType = archeTypeID;
			for (int i = 0; i < def.num; ++i) {
				_mappings[_offset + i] = EMPTY_ID;
			}
			_offset += def.num;
			return idx.id;
		}
		return NO_ID;
	}

	ID createArcheType(ArcheType* infos, int num) {
		ArcheTypeDefinition def;
		for (int i = 0; i < num; ++i) {
			def.infos.push_back(infos[i].info);
		}
		def.num = num;
		_archeTypes.push_back(def);
		return _archeTypes.size() - 1;
	}

	void debug();

private:

	bool findMatchingStream(const std::type_info* type, int* streamIndex, int* channelIndex) {
		for (size_t i = 0; i < _streams.size(); ++i) {
			int channels = _streams[i]->_num_blocks;
			for (int j = 0; j < channels; ++j) {
				if (*(_streams[i]->getTypeInfo(j)) == *type) {
					*streamIndex = i;
					*channelIndex = j;
					return true;
				}
			}
		}
		return false;
	}

	int findFreeId() {
		for (int i = 0; i < 4096; ++i) {
			if (_indices[i].index == EMPTY_ID) {
				return i;
			}
		}
		return -1;
	}
	int createStream(ds::ChannelArrayDefinition* defs, int num) {
		ds::ChannelArray* ar = new ds::ChannelArray;
		ar->init(defs, num);
		_streams.push_back(ar);
		return _streams.size() - 1;
	}
	std::vector<ArcheTypeDefinition> _archeTypes;
	std::vector<ds::ChannelArray*> _streams;
	EntityIndex _indices[4096];
	ID _mappings[4096];
	int _offset;
	
};

