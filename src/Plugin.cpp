#include "Plugin.h"
#include <string.h>

struct Plugin {
	const char* name;
	void* data;
};

static Plugin PLUGINS[32];
static int index = 0;

void add(const char* name, void* interf) {
	Plugin& p = PLUGINS[index++];
	p.name = name;
	p.data = interf;
};

void* get(const char* name) {
	for (int i = 0; i < index; ++i) {
		if (strcmp(PLUGINS[i].name, name) == 0) {
			return PLUGINS[i].data;
		}
	}
	return 0;
}

static ds_api_registry REGISTRY = { add, get };

ds_api_registry create_registry() {
	return REGISTRY;
}


static void create(struct ds_test_plugin_data* data) {

}

int add(struct ds_test_plugin_data* data, int a, int b) {
	return a + b;
}

struct ds_test_plugin_data {
	float values[32];
	int capacity;
	int index;
};

static ds_test_plugin_data data;

static struct ds_test_plugin TMP = { &data, create, add };

void load_test_plugin(struct ds_api_registry* registry) {
	registry->add(DS_TEST_PLUGIN_NAME, &TMP);
}

ds_test_plugin get_plugin() {
	return TMP;
}
