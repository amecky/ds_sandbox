#include "EventStream.h"

namespace ds {

EventStream::EventStream() {
	m_Data = new char[4096];
	reset();
}

EventStream::~EventStream() {
	delete[] m_Data;
}

// -------------------------------------------------------
// reset
// -------------------------------------------------------
void EventStream::reset() {
	_mappings.clear();
	_index = 0;
}

// -------------------------------------------------------
// add event
// -------------------------------------------------------
void EventStream::add(uint32_t type, void* p, size_t size) {
	addHeader(type, size);	
	char* data = m_Data + _index + sizeof(EventHeader);
	memcpy(data,p,size);
	_mappings.push_back(_index);
	_index += sizeof(EventHeader) + size;
}

// -------------------------------------------------------
// add event
// -------------------------------------------------------
void EventStream::add(uint32_t type) {
	addHeader(type, 0);
	char* data = m_Data + _index;
	_mappings.push_back(_index);
	_index += sizeof(EventHeader);
}

// -------------------------------------------------------
// add header
// -------------------------------------------------------
void EventStream::addHeader(uint32_t type, size_t size) {
	//LOG << "creating event - type: " << type << " size: " << size;
	EventHeader header;
	header.id = _mappings.size();;
	header.size = size;
	header.type = type;
	char* data = m_Data + _index;
	memcpy(data, &header, sizeof(EventHeader));
}

// -------------------------------------------------------
// get
// -------------------------------------------------------
const bool EventStream::get(uint32_t index, void* p) const {
	int lookup = _mappings[index];
	char* data = m_Data + lookup;
	EventHeader* header = (EventHeader*)data;
	data += sizeof(EventHeader);
	memcpy(p,data,header->size);
	return true;
}

// -------------------------------------------------------
// get type
// -------------------------------------------------------
const int EventStream::getType(uint32_t index) const {
	int lookup = _mappings[index];
	char* data = m_Data + lookup;
	EventHeader* header = (EventHeader*)data;
	return header->type;
}

// -------------------------------------------------------
// contains type
// -------------------------------------------------------
const bool EventStream::containsType(uint32_t type) const {
	for (int i = 0; i < _mappings.size(); ++i) {
		if ( getType(i) == type ) {
			return true;
		}
	}
	return false;
}


}