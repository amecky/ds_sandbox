#include "Plugin.h"
#include "plugins\ApiRegistry.h"

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
