#pragma once

//#define DS_PERF_PANEL

namespace perfpanel {

	void show_profiler();

	void tick_histogram(float dt);

}

#ifdef DS_PERF_PANEL 
#include <ds_profiler.h>
#include <stdint.h>
#include <math.h>
#include <ds_imgui.h>

namespace perfpanel {

	struct PerfHistory {
		float values[32];
		uint16_t index;
		uint16_t lastIndex;
		float timer;
		float maxValue;

		int state;
		p2i dialogPos;
		bool showHistogram;
		bool showProfiler;

		PerfHistory() : index(0), lastIndex(0), timer(0.0f) , maxValue(0.0f), state(1) , showHistogram(true), showProfiler(true) {}
	};

	static PerfHistory _perfHistory;

	static double niceNum(double range, bool round) {
		double niceFraction = 0.0;
		double exponent = floor(log10(range));
		double fraction = range / pow(10, exponent);
		if (round) {
			if (fraction < 1.5)
				niceFraction = 1;
			else if (fraction < 3)
				niceFraction = 2;
			else if (fraction < 7)
				niceFraction = 5;
			else
				niceFraction = 10;
		}
		else {
			if (fraction <= 1)
				niceFraction = 1;
			else if (fraction <= 2)
				niceFraction = 2;
			else if (fraction <= 5)
				niceFraction = 5;
			else
				niceFraction = 10;
		}

		return niceFraction * pow(10, exponent);
	}

	static void get_nice_scaling(double minPoint, double maxPoint, int maxTicks, bool round, double* niceMin, double* niceMax, double* tickSpacing) {
		double range = niceNum(maxPoint - minPoint, false);
		*tickSpacing = niceNum(range / (maxTicks - 1), true);
		*niceMin = floor(minPoint / *tickSpacing) * *tickSpacing;
		*niceMax = ceil(maxPoint / *tickSpacing) * *tickSpacing;
	}


	void show_profiler() {
		gui::end();
		_perfHistory.dialogPos = p2i(ds::getScreenWidth() - 300, ds::getScreenHeight() - 10);
		gui::begin("Profiler", &_perfHistory.state, &_perfHistory.dialogPos, 300);
		if (_perfHistory.state == 1) {
			gui::Checkbox("Profiler", &_perfHistory.showProfiler);
			if (perf::num_events() > 0 && _perfHistory.showProfiler) {
				for (size_t i = 0; i < perf::num_events(); ++i) {
					gui::Value(perf::get_name(i), perf::avg(i), 4, 8);
				}
				gui::Value("Total", perf::avg_total(), 4, 8);
				if (gui::Button("save")) {
					FILE* fp = fopen("perf.txt", "w");
					if (fp) {
						for (size_t i = 0; i < perf::num_events(); ++i) {
							fprintf(fp, "%s %2.5f %d\n", perf::get_name(i), perf::avg(i), perf::num_calls(i));
							//LOG_DEBUG("%s %2.5f %d", perf::get_name(i), perf::avg(i), perf::num_calls(i));
						}
						fprintf(fp, "Total %2.5f\n", perf::avg_total());
						//LOG_DEBUG("Total %2.5f", perf::avg_total());
						fclose(fp);
					}
				}
			}
			gui::Checkbox("Histogram", &_perfHistory.showHistogram);
			if (_perfHistory.maxValue > 0.0f && _perfHistory.showHistogram) {
				double niceMin = 0.0;
				double niceMax = 0.0;
				double tickSpacing = 0.0;
				get_nice_scaling(0.0, _perfHistory.maxValue, 5, true, &niceMin, &niceMax, &tickSpacing);
				gui::Histogram(_perfHistory.values, _perfHistory.index, 0.0f, niceMax, tickSpacing);
			}
		}
		gui::end();
	}

	void tick_histogram(float dt) {
		_perfHistory.timer += dt;
		if (_perfHistory.timer > 0.25f) {
			_perfHistory.timer -= 0.25f;
			if (_perfHistory.index + 1 >= 32) {
				for (int i = 1; i < 32; ++i) {
					_perfHistory.values[i - 1] = _perfHistory.values[i];
				}
				_perfHistory.index = 31;
			}
			float current = perf::avg_total();
			_perfHistory.values[_perfHistory.index++] = current;
			if (current > _perfHistory.maxValue) {
				_perfHistory.maxValue = current;
			}
		}
	}
}

#endif