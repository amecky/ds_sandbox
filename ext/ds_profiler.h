#pragma once

//#define DS_PROFILER_IMPLEMENTATION

namespace perf {

	void init();

	void reset();

	void finalize();

	uint16_t start(const char* name);

	void end(uint16_t index);

	void shutdown();

	void debug();

	//void save(const ds::ReportWriter& writer);

	void tickFPS(float dt);

	void incFrame();

	uint16_t num_events();

	double dt(uint16_t i);

	double avg(uint16_t i);

	const char* get_name(uint16_t i);

	double avg_total();

	int num_calls(uint16_t idx);

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

#ifdef DS_PROFILER_IMPLEMENTATION

#include <Windows.h>
#include <stdint.h>
#include <intrin.h>

namespace perf {

	struct CharBuffer {

		char* data;
		int size;
		int capacity;
		int num;

		CharBuffer();
		~CharBuffer();

		void* alloc(int sz);
		void resize(int newCap);
		int append(const char* s, int len);
		int append(const char* s);
		int append(char s);
		const char* get(int index) const;

	};

	const uint32_t FNV_Prime = 0x01000193; //   16777619
	const uint32_t FNV_Seed = 0x811C9DC5; // 2166136261

	static inline uint32_t fnv1a(const char* text, uint32_t hash = FNV_Seed) {
		const unsigned char* ptr = (const unsigned char*)text;
		while (*ptr) {
			hash = (*ptr++ ^ hash) * FNV_Prime;
		}
		return hash;
	}

	// -----------------------------------------------------------
	// zone tracker event
	// -----------------------------------------------------------
	struct ZoneTrackerEvent {
		uint32_t hash;
		int parent;
		int name_index;
		uint64_t startTicks;
	};

	const static uint16_t MAX_ENTRIES = 256;
	// -----------------------------------------------------------
	// Zone tracker context
	// -----------------------------------------------------------
	// https://github.com/GarageGames/Torque2D/blob/master/engine/source/debug/profiler.cc
	struct ZoneTrackerContext {

		ZoneTrackerEvent trackerEvents[MAX_ENTRIES];
		double current[MAX_ENTRIES];
		double accu[MAX_ENTRIES];
		double average[MAX_ENTRIES];
		int calls[MAX_ENTRIES];
		uint64_t ticks[MAX_ENTRIES];
		double totalAverage;
		uint16_t frameCount;
		double beginAverage;
		uint16_t num_entries;
		DWORD cpuTicks;
		LARGE_INTEGER frequency;
		CharBuffer names;
		int current_parent;
		int root_event;
		int ident;
		int frames;
		float fpsTimer;
		int fps;
		LARGE_INTEGER lastTime;
		uint64_t totalTicks;
	};

	static ZoneTrackerContext* zoneTrackerCtx = 0;

	static double LIToSecs(LARGE_INTEGER & L) {
		return (L.QuadPart) * 1000000.0 / zoneTrackerCtx->frequency.QuadPart;
	}

	static const uint64_t TicksPerSecond = 1000000;

	static double TicksToSeconds(uint64_t ticks) {
		return static_cast<double>(ticks) / TicksPerSecond;
	}

	static uint64_t rdtsc() {
		return __rdtsc();
	}

	// -----------------------------------------------------------
	// init
	// -----------------------------------------------------------
	void init() {
		if (zoneTrackerCtx == 0) {
			zoneTrackerCtx = new ZoneTrackerContext;
			QueryPerformanceFrequency(&zoneTrackerCtx->frequency);
			zoneTrackerCtx->cpuTicks = zoneTrackerCtx->frequency.LowPart;
		}
		zoneTrackerCtx->frames = 0;
		zoneTrackerCtx->fpsTimer = 0.0f;
		zoneTrackerCtx->fps = 0;
		zoneTrackerCtx->num_entries = 0;
		for (int i = 0; i < MAX_ENTRIES; ++i) {
			zoneTrackerCtx->current[i] = 0.0f;
			zoneTrackerCtx->accu[i] = 0.0f;
			zoneTrackerCtx->average[i] = 0.0f;
		}
		zoneTrackerCtx->totalTicks = 0;
		zoneTrackerCtx->beginAverage = TicksToSeconds(zoneTrackerCtx->totalTicks);
		zoneTrackerCtx->frameCount = 0;
		zoneTrackerCtx->totalAverage = 0.0;
	}

	static void tick_timers() {
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		uint64_t timeDelta = currentTime.QuadPart - zoneTrackerCtx->lastTime.QuadPart;
		zoneTrackerCtx->lastTime = currentTime;
		timeDelta *= TicksPerSecond;
		timeDelta /= zoneTrackerCtx->frequency.QuadPart;
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
		for (int i = 0; i < MAX_ENTRIES; ++i) {
			zoneTrackerCtx->calls[i] = 0;
		}
		zoneTrackerCtx->ident = 0;
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
		++zoneTrackerCtx->frameCount;		
		if (get_total_seconds() > zoneTrackerCtx->beginAverage + 0.5) {
			zoneTrackerCtx->totalAverage = 0.0;
			for (int i = 0; i < zoneTrackerCtx->num_entries; ++i) {
				zoneTrackerCtx->average[i] = zoneTrackerCtx->accu[i] / static_cast<float>(zoneTrackerCtx->frameCount);				
				zoneTrackerCtx->accu[i] = 0.0;
				if (i > 0) {
					zoneTrackerCtx->totalAverage += zoneTrackerCtx->average[i];
				}
			}
			zoneTrackerCtx->beginAverage = get_total_seconds();
			zoneTrackerCtx->frameCount = 0;
		}
	}

	double avg_total() {
		return zoneTrackerCtx->totalAverage;
	}

	double avg(uint16_t idx) {
		return zoneTrackerCtx->average[idx];
	}

	double avg_total(uint16_t idx) {
		return zoneTrackerCtx->average[0];
	}

	uint16_t num_events() {
		return zoneTrackerCtx->num_entries;
	}

	const ZoneTrackerEvent& get_event(uint16_t i) {
		return zoneTrackerCtx->trackerEvents[i];
	}

	int num_calls(uint16_t idx) {
		return zoneTrackerCtx->calls[idx];
	}

	const char* get_name(uint16_t i) {
		return zoneTrackerCtx->names.data + zoneTrackerCtx->trackerEvents[i].name_index;
	}

	double dt(uint16_t i) {
		return zoneTrackerCtx->current[i];
	}
	// -----------------------------------------------------------
	// debug
	// -----------------------------------------------------------
	void debug() {
		/*
		LOG << "------------------------------------------------------------";
		LOG << " Percent | Accu       | Name";
		LOG << "------------------------------------------------------------";
		float norm = zoneTrackerCtx->events[0].duration;
		char buffer[256];
		char p[10];
		std::string line;
		for (uint32_t i = 0; i < zoneTrackerCtx->events.size(); ++i) {
		const ZoneTrackerEvent& event = zoneTrackerCtx->events[i];
		int ident = event.ident * 2;
		float per = event.duration / norm * 100.0f;
		ds::string::formatPercentage(per, p);
		sprintf(buffer, "%s  | %3.8f | ", p, event.duration);
		line = buffer;
		for (int j = 0; j < ident; ++j) {
		line += " ";
		}
		const char* n = zoneTrackerCtx->names.data + zoneTrackerCtx->events[i].name_index;
		LOG << line << " " << n;
		}
		LOG << "------------------------------------------------------------";
		*/
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
	// increment frame
	// -----------------------------------------------------------
	void incFrame() {
		++zoneTrackerCtx->frames;
	}

	// -----------------------------------------------------------
	// find by hash
	// -----------------------------------------------------------
	int findHash(const uint32_t& hash) {
		for (uint32_t i = 0; i < zoneTrackerCtx->num_entries; ++i) {
			if (zoneTrackerCtx->trackerEvents[i].hash == hash) {
				return i;
			}
		}
		return -1;
	}

	// -----------------------------------------------------------
	// start zone tracking
	// -----------------------------------------------------------
	uint16_t start(const char* name) {
		uint32_t hash = fnv1a(name);
		int idx = findHash(hash);
		if (idx == -1) {
			ZoneTrackerEvent& event = zoneTrackerCtx->trackerEvents[zoneTrackerCtx->num_entries++];
			event.startTicks = rdtsc();
			event.hash = fnv1a(name);
			event.name_index = zoneTrackerCtx->names.size;
			int l = strlen(name);
			if (zoneTrackerCtx->names.size + l > zoneTrackerCtx->names.capacity) {
				zoneTrackerCtx->names.resize(zoneTrackerCtx->names.capacity + MAX_ENTRIES);
			}
			zoneTrackerCtx->names.append(name, l);
			event.parent = zoneTrackerCtx->current_parent;
			idx = zoneTrackerCtx->num_entries - 1;
		}
		else {
			ZoneTrackerEvent& event = zoneTrackerCtx->trackerEvents[idx];
			event.startTicks = rdtsc();
			event.parent = zoneTrackerCtx->current_parent;
			zoneTrackerCtx->current_parent = idx;
		}
		++zoneTrackerCtx->calls[idx];
		return idx;
	}

	// -----------------------------------------------------------
	// end zone tracking
	// -----------------------------------------------------------
	void end(uint16_t index) {
		ZoneTrackerEvent& event = zoneTrackerCtx->trackerEvents[index];
		zoneTrackerCtx->ticks[index] = rdtsc() - event.startTicks;		
		double d = static_cast<double>(zoneTrackerCtx->ticks[index]) / static_cast<double>(zoneTrackerCtx->cpuTicks);
		zoneTrackerCtx->current[index] = d;
		zoneTrackerCtx->accu[index] += zoneTrackerCtx->current[index];
		if (zoneTrackerCtx->trackerEvents[zoneTrackerCtx->current_parent].parent != -1) {
			zoneTrackerCtx->current_parent = zoneTrackerCtx->trackerEvents[zoneTrackerCtx->current_parent].parent;
		}
		--zoneTrackerCtx->ident;
	}

	// -----------------------------------------------------------
	// shutdown
	// -----------------------------------------------------------
	void shutdown() {
		delete zoneTrackerCtx;
	}

	CharBuffer::CharBuffer() : data(nullptr), size(0), capacity(0), num(0) {}

	CharBuffer::~CharBuffer() {
		if (data != nullptr) {
			delete[] data;
		}
	}

	void* CharBuffer::alloc(int sz) {
		if (size + sz > capacity) {
			int d = capacity * 2 + 8;
			if (d < sz) {
				d = sz * 2 + 8;
			}
			resize(d);
		}
		auto res = data + size;
		size += sz;
		int d = sz / 4;
		if (d == 0) {
			d = 1;
		}
		num += d;
		return res;
	}

	void CharBuffer::resize(int newCap) {
		if (newCap > capacity) {
			char* tmp = new char[newCap];
			if (data != nullptr) {
				memcpy(tmp, data, size);
				delete[] data;
			}
			capacity = newCap;
			data = tmp;
		}
	}

	const char* CharBuffer::get(int index) const {
		return data + index;
	}

	int CharBuffer::append(const char* s, int len) {
		if (size + len + 1 > capacity) {
			resize(capacity + len + 1 + 8);
		}
		const char* t = s;
		int ret = size;
		for (int i = 0; i < len; ++i) {
			data[size++] = *t;
			++t;
		}
		data[size++] = '\0';
		return ret;
	}

	int CharBuffer::append(const char* s) {
		int len = strlen(s);
		return append(s, len);
	}

	int CharBuffer::append(char s) {
		if (size + 1 > capacity) {
			resize(capacity + 9);
		}
		int ret = size;
		data[size++] = s;
		data[size++] = '\0';
		return ret;
	}
}

#endif