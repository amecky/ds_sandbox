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

	float dt(uint16_t i);

	float avg(uint16_t i);

	const char* get_name(uint16_t i);

	float avg_total();

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
		LARGE_INTEGER started;
		int name_index;
	};

	const static uint16_t MAX_ENTRIES = 256;
	// -----------------------------------------------------------
	// Zone tracker context
	// -----------------------------------------------------------
	struct ZoneTrackerContext {

		ZoneTrackerEvent trackerEvents[MAX_ENTRIES];
		float current[MAX_ENTRIES];
		float accu[MAX_ENTRIES];
		float average[MAX_ENTRIES];
		uint16_t frameCount;
		float beginAverage;
		uint16_t num_entries;
		
		LARGE_INTEGER frequency;
		CharBuffer names;
		int current_parent;
		int root_event;
		int ident;
		int frames;
		float fpsTimer;
		int fps;
		LONGLONG overhead;


		//LARGE_INTEGER timerFrequency;
		LARGE_INTEGER lastTime;
		// Derived timing data uses a canonical tick format. 
		//uint64_t elapsedTicks;
		uint64_t totalTicks;
		//uint64_t leftOverTicks;

		// Members for tracking the framerate. 
		//uint32_t framesPerSecond;
		//uint32_t framesThisSecond;
		//uint64_t secondCounter;
		//uint64_t maxDelta;
	};

	static ZoneTrackerContext* zoneTrackerCtx = 0;

	static double LIToSecs(LARGE_INTEGER & L) {
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
		zoneTrackerCtx->num_entries = 0;
		for (int i = 0; i < MAX_ENTRIES; ++i) {
			zoneTrackerCtx->average[i] = 0.0f;
		}

		zoneTrackerCtx->totalTicks = 0;
		zoneTrackerCtx->beginAverage = TicksToSeconds(zoneTrackerCtx->totalTicks);
		zoneTrackerCtx->frameCount = 0;
	}



	static void tick_timers() {
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		uint64_t timeDelta = currentTime.QuadPart - zoneTrackerCtx->lastTime.QuadPart;

		zoneTrackerCtx->lastTime = currentTime;
		// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp. 
		timeDelta *= TicksPerSecond;
		timeDelta /= zoneTrackerCtx->frequency.QuadPart;

		uint32_t lastFrameCount = zoneTrackerCtx->frameCount;

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
		//for (int i = 0; i < MAX_ENTRIES; ++i) {
			//zoneTrackerCtx->current[i] = 0.0f;
		//}
		//zoneTrackerCtx->names.size = 0;
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
		if (get_total_seconds() > zoneTrackerCtx->beginAverage + 0.5f) {
			for (int i = 0; i < MAX_ENTRIES; ++i) {
				zoneTrackerCtx->average[i] = zoneTrackerCtx->accu[i] / static_cast<float>(zoneTrackerCtx->frameCount);				
				zoneTrackerCtx->accu[i] = 0.0f;
			}
			zoneTrackerCtx->beginAverage = get_total_seconds();
			zoneTrackerCtx->frameCount = 0;
		}
	}

	float avg(uint16_t idx) {
		return zoneTrackerCtx->average[idx];
	}

	float avg_total(uint16_t idx) {
		return zoneTrackerCtx->average[0];
	}

	uint16_t num_events() {
		return zoneTrackerCtx->num_entries;
	}

	const ZoneTrackerEvent& get_event(uint16_t i) {
		return zoneTrackerCtx->trackerEvents[i];
	}

	const char* get_name(uint16_t i) {
		return zoneTrackerCtx->names.data + zoneTrackerCtx->trackerEvents[i].name_index;
	}

	float dt(uint16_t i) {
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
			event.hash = fnv1a(name);
			event.name_index = zoneTrackerCtx->names.size;
			int l = strlen(name);
			if (zoneTrackerCtx->names.size + l > zoneTrackerCtx->names.capacity) {
				zoneTrackerCtx->names.resize(zoneTrackerCtx->names.capacity + MAX_ENTRIES);
			}
			zoneTrackerCtx->names.append(name, l);
			idx = zoneTrackerCtx->num_entries - 1;
		}
		ZoneTrackerEvent& event = zoneTrackerCtx->trackerEvents[idx];
		event.parent = zoneTrackerCtx->current_parent;
		QueryPerformanceCounter(&event.started);
		zoneTrackerCtx->current_parent = idx;
		return idx;
	}

	// -----------------------------------------------------------
	// end zone tracking
	// -----------------------------------------------------------
	void end(uint16_t index) {
		ZoneTrackerEvent& event = zoneTrackerCtx->trackerEvents[index];
		LARGE_INTEGER EndingTime;
		QueryPerformanceCounter(&EndingTime);
		LARGE_INTEGER time;
		time.QuadPart = EndingTime.QuadPart - event.started.QuadPart;
		zoneTrackerCtx->current[index] = LIToSecs(time);
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