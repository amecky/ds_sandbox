#pragma once

#define DS_TEST_PLUGIN_NAME "ds_test_plugin"

struct ds_test_plugin {

	struct ds_test_plugin_data* data;

	void (*create)(struct ds_test_plugin_data* data);

	int (*add)(struct ds_test_plugin_data* data, int a, int b);

};

ds_test_plugin get_plugin();

void load_test_plugin(struct ds_api_registry* registry);



