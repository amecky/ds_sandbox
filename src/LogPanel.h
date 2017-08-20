#pragma once
#include <stdint.h>

namespace logpanel {

	void init();

	void add_line(const char* message);

	uint16_t get_num_lines();

	const char* get_line(uint16_t index);

	void draw_gui(int lines);

	void shutdown();
}