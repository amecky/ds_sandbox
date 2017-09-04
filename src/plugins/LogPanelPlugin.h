#pragma once
#include <stdint.h>

#define LOG_PANEL_PLUGIN_NAME "log_panel_plugin"

struct log_panel_plugin {

	void (*add_line)(const char* message);

	uint16_t (*get_num_lines)();

	const char* (*get_line)(uint16_t index);

	void (*draw_gui)(uint16_t lines);

};

void load_logpanel_plugin(struct ds_api_registry* registry);

void unload_logpanel_plugin(struct ds_api_registry* registry);



