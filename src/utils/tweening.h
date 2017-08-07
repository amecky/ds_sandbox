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

	float easeSinus(float t, float start, float end, float duration);

	// sine

	float easeInSine(float t, float start, float end, float duration);

	float easeOutSine(float t, float start, float end, float duration);

	float easeInOutSine(float t, float start, float end, float duration);

	// quad

	float easeInQuad(float t, float start, float end, float duration);

	float easeOutQuad(float t, float start, float end, float duration);

	float easeInOutQuad(float t, float start, float end, float duration);

	// quint

	float easeInQuint(float t, float start, float end, float duration);

	float easeOutQuint(float t, float start, float end, float duration);

	float easeInOutQuint(float t, float start, float end, float duration);

	// expo

	float easeInExpo(float t, float start, float end, float duration);

	float easeOutExpo(float t, float start, float end, float duration);

	float easeInOutExpo(float t, float start, float end, float duration);

	// quart

	float easeInQuart(float t, float start,float end,float duration);

	float easeOutQuart(float t, float start, float end, float duration);

	float easeInOutQuart(float t, float start, float end, float duration);

	// back

	float easeInBack(float t, float start, float end, float duration);

	float easeOutBack(float t, float start, float end, float duration);

	float easeInOutBack(float t, float start, float end, float duration);

	// cubic
	
	float easeInCubic(float t, float start, float end, float duration);

	float easeOutCubic(float t, float start, float end, float duration);

	float easeInOutCubic(float t, float start, float end, float duration);

	// elastic

	float easeInElastic(float t, float start, float end, float duration);

	float easeOutElastic(float t, float start, float end, float duration);

	float easeInOutElastic(float t, float start, float end, float duration);

	// bounce

	float easeInBounce(float t, float start, float end, float duration);

	float easeOutBounce(float t, float start, float end, float duration);

	float easeInOutBounce(float t, float start, float end, float duration);

	// circ

	float easeInCirc(float t, float start, float end, float duration);

	float easeOutCirc(float t, float start, float end, float duration);

	float easeInOutCirc(float t, float start, float end, float duration);

	TweeningType get_by_index(int index);

	float interpolate(TweeningType type, float start, float end, float t, float duration);

	ds::vec2 interpolate(TweeningType type,const ds::vec2& start,const ds::vec2& end,float t,float duration);

	ds::vec3 interpolate(TweeningType type, const ds::vec3& start, const ds::vec3& end, float t, float duration);

	ds::vec4 interpolate(TweeningType type, const ds::vec4& start, const ds::vec4& end, float t, float duration);

	ds::Color interpolate(TweeningType type, const ds::Color& start, const ds::Color& end, float t, float duration);

	const static TweeningDescriptor DESCRIPTORS[] = {
		{ linear,     "Linear" },		
		{ easeSinus, "easeSinus" },
		{ easeInSine, "easeInSine" },
		{ easeOutSine, "easeOutSine" },
		{ easeInOutSine, "easeInOutSine" },
		{ easeInQuad, "easeInQuad" },
		{ easeOutQuad, "easeOutQuad" },
		{ easeInOutQuad, "easeInOutQuad" },
		{ easeInQuart, "easeInQuart" },
		{ easeOutQuart, "easeOutQuart" },
		{ easeInOutQuart, "easeInOutQuart" },
		{ easeInQuint, "easeInQuint" },
		{ easeOutQuint, "easeOutQuint" },
		{ easeInOutQuint, "easeInOutQuint" },
		{ easeInExpo, "easeInExpo" },
		{ easeOutExpo, "easeOutExpo" },
		{ easeInOutExpo, "easeInOutExpo" },
		{ easeInBack, "easeInBack" },
		{ easeOutBack, "easeOutBack" },
		{ easeInOutBack, "easeInOutBack" },
		{ easeInCubic, "easeInCubic" },
		{ easeOutCubic, "easeOutCubic" },
		{ easeInOutCubic, "easeInOutCubic" },
		{ easeInCirc, "easeInCirc" },
		{ easeOutCirc, "easeOutCirc" },
		{ easeInOutCirc, "easeInOutCirc" },
		{ easeInElastic, "easeInElastic" },
		{ easeOutElastic, "easeOutElastic" },
		{ easeInOutElastic, "easeInOutElastic" },
		{ easeInBounce, "easeInBounce" },
		{ easeOutBounce, "easeOutBounce" },
		{ easeInOutBounce, "easeInOutBounce" }
	};
}

