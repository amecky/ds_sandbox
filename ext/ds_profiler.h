#pragma once

//#define DS_PROFILER

namespace perf {
	
	void init();

	void reset();

	void finalize();

	int start(const char* name);

	void end(int index);

	void shutdown();

	void tickFPS(float dt);

	float get_current_total_time();

	int num_events();

	float dt(int idx);

	float avg(int idx);

	const char* get_name(int idx);

	class ZoneTracker {

	public:
		ZoneTracker(const char* name) {
			_index = perf::start(name);
		}
		~ZoneTracker() {
			perf::end(_index);
		}
	private:
		int _index;
	};
}

#ifdef DS_PROFILER
#include <Windows.h>
#include <stdint.h>
#include <vector>

namespace perf {

	class NameHashBuffer {

	public:
		NameHashBuffer();
		~NameHashBuffer();				
		int append(const char* s, int len);
		int append(const char* s);
		const char* get(int index) const;
		void debug();
		bool contains(const char* name) const;
		int find(const char* name) const;
		int num() const {
			return _num_hashes;
		}
	private:
		int find(uint32_t hash) const;
		void* alloc(int sz);
		void resize(int newCap);
		void resize_hashes(int newCap);
		char* _data;
		int _size;
		int _capacity;
		int _num;
		uint32_t* _hashes;
		uint32_t* _indices;
		int _hash_capacity;
		int _num_hashes;

	};

	const uint32_t FNV_Prime = 0x01000193; //   16777619
	const uint32_t FNV_Seed = 0x811C9DC5; // 2166136261

	inline uint32_t fnv1a(const char* text, uint32_t hash = FNV_Seed) {
		const unsigned char* ptr = (const unsigned char*)text;
		while (*ptr) {
			hash = (*ptr++ ^ hash) * FNV_Prime;
		}
		return hash;
	}
	
	const static uint16_t MAX_ENTRIES = 1024;
	// -----------------------------------------------------------
	// Zone tracker context
	// -----------------------------------------------------------
	struct ZoneTrackerContext {
		LARGE_INTEGER frequency;
		NameHashBuffer names;
		int current_parent;
		int root_event;
		LARGE_INTEGER started[MAX_ENTRIES];
		double average[MAX_ENTRIES];
		double accu[MAX_ENTRIES];
		double current[MAX_ENTRIES];
		float beginAverage;
		int frames;
		float fpsTimer;
		int fps;
		LONGLONG overhead;
		uint64_t totalTicks;
		LARGE_INTEGER lastTime;
		uint16_t frameCount;
	};

	static ZoneTrackerContext* zoneTrackerCtx = 0;

	double LIToSecs(LARGE_INTEGER & L) {
		return (L.QuadPart - zoneTrackerCtx->overhead) * 1000.0 / zoneTrackerCtx->frequency.QuadPart;
	}

	static const uint64_t TicksPerSecond = 10000000;

	static double TicksToSeconds(uint64_t ticks) {
		return static_cast<double>(ticks) / TicksPerSecond;
	}

	// -----------------------------------------------------------
	// init
	// -----------------------------------------------------------
	void init() {
		if (zoneTrackerCtx == 0) {
			zoneTrackerCtx = new ZoneTrackerContext;
			QueryPerformanceFrequency(&zoneTrackerCtx->frequency);
			LARGE_INTEGER start;
			LARGE_INTEGER stop;
			QueryPerformanceCounter(&start);
			QueryPerformanceCounter(&stop);
			zoneTrackerCtx->overhead = stop.QuadPart - start.QuadPart;
		}
		zoneTrackerCtx->frames = 0;
		zoneTrackerCtx->fpsTimer = 0.0f;
		zoneTrackerCtx->fps = 0;
		zoneTrackerCtx->totalTicks = 0;
		zoneTrackerCtx->beginAverage = 0.0f;
		zoneTrackerCtx->frameCount = 0;
		for (int i = 0; i < MAX_ENTRIES; ++i) {
			zoneTrackerCtx->accu[i] = 0.0f;
			zoneTrackerCtx->current[i] = 0.0f;
			zoneTrackerCtx->average[i] = 0.0f;
		}
	}

	static void tick_timers() {
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		uint64_t timeDelta = currentTime.QuadPart - zoneTrackerCtx->lastTime.QuadPart;

		zoneTrackerCtx->lastTime = currentTime;
		// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp. 
		timeDelta *= TicksPerSecond;
		timeDelta /= zoneTrackerCtx->frequency.QuadPart;

		++zoneTrackerCtx->frameCount;

		// Variable timestep update logic. 
		zoneTrackerCtx->totalTicks += timeDelta;

	}

	static double get_total_seconds() {
		return TicksToSeconds(zoneTrackerCtx->totalTicks);
	}
	
	// -----------------------------------------------------------
	// reset
	// -----------------------------------------------------------
	void reset() {
		tick_timers();
		// create root event
		zoneTrackerCtx->current_parent = -1;
		zoneTrackerCtx->root_event = start("ROOT");
		zoneTrackerCtx->current_parent = zoneTrackerCtx->root_event;
	}

	// -----------------------------------------------------------
	// finalize
	// -----------------------------------------------------------
	void finalize() {
		end(zoneTrackerCtx->root_event);
		++zoneTrackerCtx->frames;
		if ((get_total_seconds() > zoneTrackerCtx->beginAverage + 0.5f)) {
			for (int i = 0; i < MAX_ENTRIES; ++i) {
				zoneTrackerCtx->average[i] = zoneTrackerCtx->accu[i] / static_cast<float>(zoneTrackerCtx->frameCount);
				zoneTrackerCtx->accu[i] = 0.0f;
			}
			zoneTrackerCtx->beginAverage = get_total_seconds();
			zoneTrackerCtx->frameCount = 0;
		}
	}
	
	// -----------------------------------------------------------
	// tick FPS
	// -----------------------------------------------------------
	void tickFPS(float dt) {
		zoneTrackerCtx->fpsTimer += dt;
		if (zoneTrackerCtx->fpsTimer >= 1.0f) {
			zoneTrackerCtx->fpsTimer -= 1.0f;
			zoneTrackerCtx->fps = zoneTrackerCtx->frames;
			zoneTrackerCtx->frames = 0;
		}
	}

	// -----------------------------------------------------------
	// start zone tracking
	// -----------------------------------------------------------
	int start(const char* name) {
		int idx = zoneTrackerCtx->names.find(name);				
		if (idx == -1) {
			idx = zoneTrackerCtx->names.append(name);
			QueryPerformanceCounter(&zoneTrackerCtx->started[idx]);
		}
		else {
			QueryPerformanceCounter(&zoneTrackerCtx->started[idx]);
		}
		zoneTrackerCtx->current_parent = idx;
		return idx;
	}

	// -----------------------------------------------------------
	// end zone tracking
	// -----------------------------------------------------------
	void end(int index) {

		LARGE_INTEGER started = zoneTrackerCtx->started[index];
		LARGE_INTEGER EndingTime;
		QueryPerformanceCounter(&EndingTime);
		uint64_t timeDelta = EndingTime.QuadPart - started.QuadPart;
		timeDelta *= TicksPerSecond;
		timeDelta /= zoneTrackerCtx->frequency.QuadPart;
		zoneTrackerCtx->current[index] = timeDelta;
		zoneTrackerCtx->accu[index] += zoneTrackerCtx->current[index] / 10000;
	}

	// -----------------------------------------------------------
	// number of events
	// -----------------------------------------------------------
	int num_events() {
		return zoneTrackerCtx->names.num();
	}

	// -----------------------------------------------------------
	// duration at index
	// -----------------------------------------------------------
	float dt(int idx) {
		return zoneTrackerCtx->current[idx];
	}

	float avg(int idx) {
		return zoneTrackerCtx->average[idx];
	}
	// -----------------------------------------------------------
	// name at index
	// -----------------------------------------------------------
	const char* get_name(int idx) {
		return zoneTrackerCtx->names.get(idx);
	}

	// -----------------------------------------------------------
	// shutdown
	// -----------------------------------------------------------
	void shutdown() {
		delete zoneTrackerCtx;
	}
	

	NameHashBuffer::NameHashBuffer() 
		: _data(0), _hashes(0), _indices(0), _size(0), _capacity(0), _num(0), _num_hashes(0) , _hash_capacity(0) {}
	/*
	CharBuffer& CharBuffer::operator =(const CharBuffer& b) {
		int sz = b.size;
		resize(sz);
		memcpy(data, b.data, sz);
		return *this;
	}
	*/
	NameHashBuffer::~NameHashBuffer() {
		if (_data != 0) {
			delete[] _data;
		}
		if (_hashes != 0) {
			delete[] _hashes;
		}
		if (_indices != 0) {
			delete[] _indices;
		}
	}

	void* NameHashBuffer::alloc(int sz) {
		if (_size + sz > _capacity) {
			int d = _capacity * 2 + 8;
			if (d < sz) {
				d = sz * 2 + 8;
			}
			resize(d);
		}
		auto res = _data + _size;
		_size += sz;
		int d = sz / 4;
		if (d == 0) {
			d = 1;
		}
		_num += d;
		return res;
	}

	void NameHashBuffer::resize(int newCap) {
		if (newCap > _capacity) {
			char* tmp = new char[newCap];
			if (_data != nullptr) {
				memcpy(tmp, _data, _size);
				delete[] _data;
			}
			_capacity = newCap;
			_data = tmp;
		}
	}

	void NameHashBuffer::resize_hashes(int newCap) {
		if (newCap > _hash_capacity) {
			uint32_t* tmp = new uint32_t[newCap];
			if (_hashes != nullptr) {
				memcpy(tmp, _hashes, _size);
				delete[] _hashes;
			}
			_hash_capacity = newCap;
			_hashes = tmp;
			tmp = new uint32_t[newCap];
			if (_indices != nullptr) {
				memcpy(tmp, _indices, _size);
				delete[] _indices;
			}
			_indices = tmp;
		}
	}

	int NameHashBuffer::find(uint32_t hash) const {
		if (_hashes != 0) {
			for (int i = 0; i < _num_hashes; ++i) {
				if (_hashes[i] == hash) {
					return i;
				}
			}
		}
		return -1;
	}

	const char* NameHashBuffer::get(int index) const {
		return _data + _indices[index];
	}

	bool NameHashBuffer::contains(const char* name) const {
		uint32_t hash = fnv1a(name);
		int idx = find(hash);
		return idx != -1;
	}

	int NameHashBuffer::find(const char* name) const {
		uint32_t hash = fnv1a(name);
		return find(hash);
	}

	int NameHashBuffer::append(const char* s, int len) {
		uint32_t hash = fnv1a(s);
		int idx = find(hash);
		if (idx != -1) {
			return idx;
		}
		if (_size + len + 1 > _capacity) {
			resize(_capacity + len + 1 + 8);
		}
		if (_num_hashes + 1 > _hash_capacity) {
			resize_hashes(_hash_capacity * 2 + 8);
		}
		const char* t = s;
		int ret = _size;
		for (int i = 0; i < len; ++i) {
			_data[_size++] = *t;
			++t;
		}
		_data[_size++] = '\0';
		_indices[_num_hashes] = ret;
		_hashes[_num_hashes] = hash;
		++_num_hashes;
		return _num_hashes - 1;
	}

	void NameHashBuffer::debug() {
		printf("entries: %d\n", _num_hashes);
		for (int i = 0; i < _num_hashes; ++i) {
			printf("%d : %s (%d) - %d\n", i, _data + _indices[i], _indices[i], _hashes[i]);
		}
	}

	int NameHashBuffer::append(const char* s) {
		int len = strlen(s);
		return append(s, len);
	}
	/*
	int NameHashBuffer::append(char s) {
		if (_size + 1 > _capacity) {
			resize(_capacity + 9);
		}
		int ret = _size;
		_data[_size++] = s;
		_data[_size++] = '\0';
		return ret;
	}
	*/
}
#endif
