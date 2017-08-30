#pragma once

namespace perpanel {

	void show_profiler();

	void show_histogram();

	void tick_histogram(float dt);

}

#ifdef DS_PERF_PANEL 
#include <ds_profiler.h>

namespace perpanel {

	struct PerfHistory {
		float values[32];
		uint16_t index;
		uint16_t max;
		float timer;

		PerfHistory() : index(0), max(0), timer(0.0f) {}
	};

	static PerfHistory _perfHistory;

	void show_profiler() {
		p2i sp(0, lines * 20 - 10);
		gui::begin(sp);
		if (perf::num_events() > 0) {
			for (size_t i = 0; i < perf::num_events(); ++i) {
				gui::Value(perf::get_name(i), perf::avg(i), 4, 8);
			}
			gui::Value("Total", perf::avg_total(), 4, 8);
		}
		if (gui::Button("save")) {
			FILE* fp = fopen("perf.txt", "w");
			if (fp) {
				for (size_t i = 0; i < perf::num_events(); ++i) {
					fprintf(fp, "%s %2.5f %d\n", perf::get_name(i), perf::avg(i), perf::num_calls(i));
					LOG_DEBUG("%s %2.5f %d", perf::get_name(i), perf::avg(i), perf::num_calls(i));
				}
				fprintf(fp, "Total %2.5f\n", perf::avg_total());
				LOG_DEBUG("Total %2.5f", perf::avg_total());
				fclose(fp);
			}
		}
		gui::end();
	}

	void show_histogram();

	void tick_histogram(float dt) {
		_perfHistory.timer += dt;
		if (_perfHistory.timer > 0.25f) {
			_perfHistory.timer -= 0.25f;
			if (_perfHistory.index + 1 >= 32) {
				for (int i = 1; i < 32; ++i) {
					_perfHistory.values[i - 1] = _perfHistory.values[i];
				}
				_perfHistory.index = 30;
			}
			_perfHistory.values[_perfHistory.index++] = perf::avg_total();
		}
	}
}

#endif