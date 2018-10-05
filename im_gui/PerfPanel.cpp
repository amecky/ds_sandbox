#include "PerfPanel.h"
#include <diesel.h>
#include <ds_imgui.h>
#include <ds_profiler.h>

// --------------------------------------------------------
// const
// --------------------------------------------------------
PerfPanel::PerfPanel() {
	_num = 0;
	for (int i = 0; i < 16; ++i) {
		_data[i] = 0.0f;
	}
	_timer = 0.0f;
}

// --------------------------------------------------------
// render
// --------------------------------------------------------
void PerfPanel::render() {
	if (gui::begin("Performance", 0)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		for (int i = 0; i < perf::num_events(); ++i) {
			gui::Value(perf::get_name(i), perf::avg(i), "%2.6f");
		}
		gui::Histogram(_data, 16, 0.0f, 16.0f, 4.0f, 15, 100);
	}
}

// --------------------------------------------------------
// tick
// --------------------------------------------------------
void PerfPanel::tick(float dt) {
	_timer += ds::getElapsedSeconds();
	if (_timer > 0.5f) {
		_data[_num++] = perf::avg(0);
		_timer -= 0.5f;
		if (_num >= 16) {
			for (int i = 14; i >= 0; i--) {
				_data[i] = _data[i + 1];
			}
			_num = 15;
		}
	}
}