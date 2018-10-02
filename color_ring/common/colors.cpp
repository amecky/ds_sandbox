#include "colors.h"

namespace color {

	float getColorComponent(float temp1, float temp2, float temp3) {
		if (temp3 < 0.0f)
			temp3 += 1.0f;
		else if (temp3 > 1.0f)
			temp3 -= 1.0f;

		if (temp3 < 1.0f / 6.0f)
			return temp1 + (temp2 - temp1) * 6.0f * temp3;
		else if (temp3 < 0.5f)
			return temp2;
		else if (temp3 < 2.0f / 3.0f)
			return temp1 + ((temp2 - temp1) * ((2.0f / 3.0f) - temp3) * 6.0f);
		else
			return temp1;
	}

	ds::Color hsl2rgb(const HSL& hsl) {
		float r = 0.0f, g = 0.0f, b = 0.0f;
		float l = hsl.l / 100.0f;
		float s = hsl.s / 100.0f;
		float h = hsl.h / 360.0f;
		if (l != 0.0f) {
			if (s == 0.0f) {
				return ds::Color(255,255,255,255);
			}
			else {
				float temp2;
				if (l < 0.5)
					temp2 = l * (1.0f + s);
				else
					temp2 = l + s - (l * s);

				double temp1 = 2.0f * l - temp2;

				r = getColorComponent(temp1, temp2, h + 1.0f / 3.0f);
				g = getColorComponent(temp1, temp2, h);
				b = getColorComponent(temp1, temp2, h - 1.0f / 3.0f);
			}
		}
		return ds::Color(r, g, b, 1.0f);
	}

}