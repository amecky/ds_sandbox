#pragma once
#include "diesel.h"

// http://gizma.com/easing/
namespace tweening {

	typedef float (*TweeningType)(float,float,float,float);

	struct TweeningDescriptor {
		TweeningType type;
		const char* name;
	};

	float linear(float t,float start,float end,float duration);

	float easeInQuad(float t, float start, float end, float duration);

	float easeSinus(float t, float start, float end, float duration);

	float easeOutQuad(float t, float start, float end, float duration);

	float easeInOutQuad(float t, float start, float end, float duration);

	float easeInQuart(float t, float start,float end,float duration);

	float easeInBack(float t, float start, float end, float duration);

	float easeOutBack(float t, float start, float end, float duration);

	float easeInOutBack(float t, float start, float end, float duration);

	float easeInCubic(float t, float start, float end, float duration);

	float easeOutCubic(float t, float start, float end, float duration);

	float easeInOutCubic(float t, float start, float end, float duration);

	float easeInElastic(float t, float start, float end, float duration);

	float easeOutElastic(float t, float start, float end, float duration);

	float easeInOutElastic(float t, float start, float end, float duration);

	float easeInBounce(float t, float start, float end, float duration);

	float easeOutBounce(float t, float start, float end, float duration);

	float easeInOutBounce(float t, float start, float end, float duration);

	TweeningType get_by_index(int index);

	float interpolate(TweeningType type, float start, float end, float t, float duration);

	ds::vec2 interpolate(TweeningType type,const ds::vec2& start,const ds::vec2& end,float t,float duration);

	ds::vec3 interpolate(TweeningType type, const ds::vec3& start, const ds::vec3& end, float t, float duration);

	ds::vec4 interpolate(TweeningType type, const ds::vec4& start, const ds::vec4& end, float t, float duration);

	ds::Color interpolate(TweeningType type, const ds::Color& start, const ds::Color& end, float t, float duration);

	const static TweeningDescriptor DESCRIPTORS[] = {
		{ linear,     "Linear" },
		{ easeInQuad, "easeInQuad" },
		{ easeSinus, "easeSinus" },
		{ easeOutQuad, "easeOutQuad" },
		{ easeInOutQuad, "easeInOutQuad" },
		{ easeInQuart, "easeInQuart" },
		{ easeInBack, "easeInBack" },
		{ easeOutBack, "easeOutBack" },
		{ easeInOutBack, "easeInOutBack" },
		{ easeInCubic, "easeInCubic" },
		{ easeOutCubic, "easeOutCubic" },
		{ easeInOutCubic, "easeInOutCubic" },
		{ easeInElastic, "easeInElastic" },
		{ easeOutElastic, "easeOutElastic" },
		{ easeInOutElastic, "easeInOutElastic" },
		{ easeInBounce, "easeInBounce" },
		{ easeOutBounce, "easeOutBounce" },
		{ easeInOutBounce, "easeInOutBounce" }
	};
}

