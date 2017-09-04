#include "ApiRegistry.h"
#include <string.h>
#include <vector>

const uint32_t FNV_Prime = 0x01000193; //   16777619
const uint32_t FNV_Seed = 0x811C9DC5; // 2166136261

inline uint32_t fnv1a(const char* text, uint32_t hash = FNV_Seed) {
	const unsigned char* ptr = (const unsigned char*)text;
	while (*ptr) {
		hash = (*ptr++ ^ hash) * FNV_Prime;
	}
	return hash;
}

struct Plugin {
	const char* name;
	void* data;
	uint32_t hash;
};

struct RegistryContext {

	std::vector<Plugin> plugins;
};

static RegistryContext* _ctx = 0;

void add(const char* name, void* interf) {
	Plugin p;
	p.name = name;
	p.data = interf;
	p.hash = fnv1a(name);
	_ctx->plugins.push_back(p);
};

void* get(const char* name) {
	uint32_t h = fnv1a(name);
	for (size_t i = 0; i < _ctx->plugins.size(); ++i) {
		if (_ctx->plugins[i].hash == h) {
			return _ctx->plugins[i].data;
		}
	}
	return 0;
}

static ds_api_registry REGISTRY = { add, get };

ds_api_registry create_registry() {
	_ctx = new RegistryContext;
	return REGISTRY;
}

void shutdown_registry() {
	if (_ctx != 0) {
		delete _ctx;
		// FIXME: call shutdown on all registered plugins???
	}
}

