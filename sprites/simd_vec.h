#pragma once
#define VM_INLINE   __forceinline 
#define SHUFFLE3(V, X,Y,Z) vec(_mm_shuffle_ps((V).m, (V).m, _MM_SHUFFLE(Z,Z,Y,X)))

#if defined(_MSC_VER) && !defined(_M_ARM) && !defined(_M_ARM64) && !defined(_M_HYBRID_X86_ARM64) && (!_MANAGED) && (!_M_CEE) && (!defined(_M_IX86_FP) || (_M_IX86_FP > 1)) && !defined(_XM_NO_INTRINSICS_) && !defined(_XM_VECTORCALL_)
#define _XM_VECTORCALL_ 1
#endif

#if _XM_VECTORCALL_
#define XM_CALLCONV __vectorcall
#else
#define XM_CALLCONV __fastcall
#endif

#include <xmmintrin.h>

struct vec {

	VM_INLINE vec() { m = _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f); }
	VM_INLINE explicit vec(const float *p) { m = _mm_set_ps(p[2], p[2], p[1], p[0]); }
	VM_INLINE explicit vec(float x, float y, float z) { m = _mm_set_ps(z, z, y, x); }
	VM_INLINE explicit vec(__m128 v) { m = v; }

	VM_INLINE float x() const { return _mm_cvtss_f32(m); }
	VM_INLINE float y() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 1, 1, 1))); }
	VM_INLINE float z() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 2, 2, 2))); }

	VM_INLINE vec yzx() const { return SHUFFLE3(*this, 1, 2, 0); }
	VM_INLINE vec zxy() const { return SHUFFLE3(*this, 2, 0, 1); }

	VM_INLINE void store(float *p) const { p[0] = x(); p[1] = y(); p[2] = z(); }

	void setX(float x) {
		m = _mm_move_ss(m, _mm_set_ss(x));
	}
	void setY(float y) {
		__m128 t = _mm_move_ss(m, _mm_set_ss(y));
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
		m = _mm_move_ss(t, m);
	}
	void setZ(float z) {
		__m128 t = _mm_move_ss(m, _mm_set_ss(z));
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
		m = _mm_move_ss(t, m);
	}

	VM_INLINE float operator[] (size_t i) const { return m.m128_f32[i]; };
	VM_INLINE float& operator[] (size_t i) { return m.m128_f32[i]; };

	__m128 m;
};



VM_INLINE vec XM_CALLCONV operator+ (vec a, vec b) { a.m = _mm_add_ps(a.m, b.m); return a; }
VM_INLINE vec XM_CALLCONV operator- (vec a, vec b) { a.m = _mm_sub_ps(a.m, b.m); return a; }
VM_INLINE vec XM_CALLCONV operator* (vec a, vec b) { a.m = _mm_mul_ps(a.m, b.m); return a; }
VM_INLINE vec XM_CALLCONV operator/ (vec a, vec b) { a.m = _mm_div_ps(a.m, b.m); return a; }
VM_INLINE vec XM_CALLCONV operator* (vec a, float b) { a.m = _mm_mul_ps(a.m, _mm_set1_ps(b)); return a; }
VM_INLINE vec XM_CALLCONV operator/ (vec a, float b) { a.m = _mm_div_ps(a.m, _mm_set1_ps(b)); return a; }
VM_INLINE vec XM_CALLCONV operator* (float a, vec b) { b.m = _mm_mul_ps(_mm_set1_ps(a), b.m); return b; }
VM_INLINE vec XM_CALLCONV operator/ (float a, vec b) { b.m = _mm_div_ps(_mm_set1_ps(a), b.m); return b; }
VM_INLINE vec& XM_CALLCONV operator+= (vec &a, vec b) { a = a + b; return a; }
VM_INLINE vec& XM_CALLCONV operator-= (vec &a, vec b) { a = a - b; return a; }
VM_INLINE vec& XM_CALLCONV operator*= (vec &a, vec b) { a = a * b; return a; }
VM_INLINE vec& XM_CALLCONV operator/= (vec &a, vec b) { a = a / b; return a; }
VM_INLINE vec& XM_CALLCONV operator*= (vec &a, float b) { a = a * b; return a; }
VM_INLINE vec& XM_CALLCONV operator/= (vec &a, float b) { a = a / b; return a; }

//VM_INLINE vec clamp(vec t, vec a, vec b) { return min(max(t, a), b); }
VM_INLINE float XM_CALLCONV sum(vec v) { return v.x() + v.y() + v.z(); }
VM_INLINE float XM_CALLCONV dot(vec a, vec b) { return sum(a*b); }
VM_INLINE float XM_CALLCONV length(vec v) { return sqrtf(dot(v, v)); }
VM_INLINE float XM_CALLCONV sqr_length(vec v) { return dot(v, v); }
VM_INLINE vec XM_CALLCONV normalize(vec v) { return v * (1.0f / length(v)); }
VM_INLINE vec XM_CALLCONV lerp(vec a, vec b, float t) { return a + (b - a)*t; }

VM_INLINE vec XM_CALLCONV cross(vec a, vec b) {
	// x  <-  a.y*b.z - a.z*b.y
	// y  <-  a.z*b.x - a.x*b.z
	// z  <-  a.x*b.y - a.y*b.x
	// We can save a shuffle by grouping it in this wacky order:
	return (a.zxy()*b - a * b.zxy()).zxy();
}

