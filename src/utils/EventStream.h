#pragma once
#include <stdint.h>
#include <vector>

namespace ds {

class EventStream {

	struct EventHeader {
		uint32_t id;
		uint32_t type;
		size_t size;
	};    

public:
	EventStream();    
	virtual ~EventStream();
	void reset();
	void add(uint32_t type);
	void add(uint32_t type, void* p, size_t size);
	const bool get(uint32_t index, void* p) const;
	const int getType(uint32_t index) const;
	const bool containsType(uint32_t type) const;
	const uint32_t num() const {
		return _mappings.size();
	}
private:
	void addHeader(uint32_t type,size_t size);
	EventStream(const EventStream& orig) {}
	char* m_Data;
	std::vector<uint32_t> _mappings;
	uint32_t _index;
};

}

