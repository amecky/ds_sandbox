#pragma once
#include <diesel.h>
// ------------------------------------------------------
// HSL
// 0 < h < 360
// 0 < s < 100
// 0 < l < 100
// ------------------------------------------------------
struct HSL {

	union {
		float values[3];
		struct {
			float h;
			float s;
			float l;
		};
	};

	HSL() : h(0.0f), s(0.0f), l(0.0f) {}
	HSL(float hue, float saturation, float luminance) : h(hue), s(saturation), l(luminance) {}

};

namespace color {

	ds::Color hsl2rgb(const HSL& hsl);

}