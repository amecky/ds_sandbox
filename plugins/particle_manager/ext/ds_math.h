#pragma once
#include <math.h>

namespace ds {

	struct vec2 {
		union {
			struct {
				float x, y;
			};
			float data[2];
		};

		vec2() : x(0.0f), y(0.0f) {}
		explicit vec2(float v) : x(v), y(v) {}
		vec2(float xx, float yy) : x(xx), y(yy) {}
		vec2(int v) {
			x = static_cast<float>(v);
			y = static_cast<float>(v);
		}
		vec2(int xx,int yy) {
			x = static_cast<float>(xx);
			y = static_cast<float>(yy);
		}
		vec2(const vec2& other) {
			x = other.x;
			y = other.y;
		}

		const float* operator() () const {
			return &data[0];
		}
	};

	struct vec3 {
		union {
			struct {
				float x, y, z;
			};
			float data[3];
		};
		vec3() : x(0.0f), y(0.0f) , z(0.0f) {}
		explicit vec3(float v) : x(v), y(v) , z(v) {}
		vec3(float xx, float yy) : x(xx), y(yy) , z(0.0f) {}
		vec3(const vec2& v) : x(v.x), y(v.y), z(0.0f) {}
		vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
		vec3(int v) {
			x = static_cast<float>(v);
			y = static_cast<float>(v);
			z = static_cast<float>(v);
		}
		vec3(int xx, int yy) {
			x = static_cast<float>(xx);
			y = static_cast<float>(yy);
			z = 0.0f;
		}
		vec3(int xx, int yy, int zz) {
			x = static_cast<float>(xx);
			y = static_cast<float>(yy);
			z = static_cast<float>(zz);
		}
		vec3(const vec3& other) {
			x = other.x;
			y = other.y;
			z = other.z;
		}

		const float* operator() () const {
			return &data[0];
		}
	};

	struct vec4 {
		union {
			struct {
				float x, y, z, w;
			};
			float data[4];
		};
		vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		explicit vec4(float v) : x(v), y(v), z(v), w(v) {}
		vec4(float xx, float yy) : x(xx), y(yy), z(0.0f) , w(0.0f) {}
		vec4(const vec2& v) : x(v.x), y(v.y), z(0.0f) , w(0.0f) {}
		vec4(const vec3& v) : x(v.x), y(v.y), z(v.z), w(0.0f) {}
		vec4(float xx, float yy, float zz) : x(xx), y(yy), z(zz), w(0.0f) {}
		vec4(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz) , w(ww) {}
		vec4(int v) {
			x = static_cast<float>(v);
			y = static_cast<float>(v);
			z = static_cast<float>(v);
			w = static_cast<float>(v);
		}
		vec4(int xx, int yy) {
			x = static_cast<float>(xx);
			y = static_cast<float>(yy);
			z = 0.0f;
			w = 0.0f;
		}
		vec4(int xx, int yy, int zz) {
			x = static_cast<float>(xx);
			y = static_cast<float>(yy);
			z = static_cast<float>(zz);
			w = 0.0f;
		}
		vec4(int xx, int yy, int zz, int ww) {
			x = static_cast<float>(xx);
			y = static_cast<float>(yy);
			z = static_cast<float>(zz);
			w = static_cast<float>(ww);
		}
		vec4(const vec4& other) {
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
		}

		const float* operator() () const {
			return &data[0];
		}
	};

	struct Color {
		union {
			struct {
				float r, g, b, a;
			};
			float data[4];
		};

		Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
		Color(float ir, float ig, float ib, float ia) : r(ir), g(ig) , b(ib), a(ia) {}
		Color(int ir, int ig, int ib, int ia) {
			r = static_cast<float>(ir) / 255.0f;
			g = static_cast<float>(ig) / 255.0f;
			b = static_cast<float>(ib) / 255.0f;
			a = static_cast<float>(ia) / 255.0f;
		}

		operator float* () {
			return &data[0];
		}

		operator const float* () const {
			return &data[0];
		}
	};

	struct matrix {

		union {
			struct {
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;

			};
			float m[4][4];
		};
		
		float& operator () (int a, int b) {
			return m[a][b];
		}
	};

	inline bool operator == (const vec2& u, const vec2& v) {
		return u.x == v.x && u.y == v.y;
	}

	inline bool operator == (const vec3& u, const vec3& v) {
		return u.x == v.x && u.y == v.y && u.z == v.z;
	}

	inline bool operator == (const vec4& u, const vec4& v) {
		return u.x == v.x && u.y == v.y && u.z == v.z && u.w == v.w;
	}

	inline bool operator != (const vec2& u, const vec2& v) {
		return u.x != v.x || u.y != v.y;
	}

	inline bool operator != (const vec3& u, const vec3& v) {
		return u.x != v.x || u.y != v.y || u.z != v.z;
	}

	inline bool operator != (const vec4& u, const vec4& v) {
		return u.x != v.x || u.y != v.y || u.z != v.z || u.w != v.w;
	}

	inline vec2 operator - (const vec2& v) {
		return{ -v.x, -v.y };
	}

	inline vec3 operator - (const vec3& v) {
		return{ -v.x, -v.y, -v.z };
	}

	inline vec4 operator - (const vec4& v) {
		return{ -v.x, -v.y, -v.z, -v.w };
	}

	inline vec2 operator - (const vec2& u, const vec2& v) {
		return{ u.x -v.x, u.y -v.y };
	}

	inline vec3 operator - (const vec3& u, const vec3& v) {
		return{ u.x - v.x, u.y - v.y, u.z - v.z };
	}

	inline vec4 operator - (const vec4& u, const vec4& v) {
		return{ u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w };
	}

	inline vec2 operator += (vec2& u, const vec2& v) {
		u.x += v.x;
		u.y += v.y;
		return u;
	}

	inline vec3 operator += (vec3& u, const vec3& v) {
		u.x += v.x;
		u.y += v.y;
		u.z += v.z;
		return u;
	}

	inline vec4 operator += (vec4& u, const vec4& v) {
		u.x += v.x;
		u.y += v.y;
		u.z += v.z;
		u.w += v.w;
		return u;
	}

	inline vec2 operator + (const vec2& u, const vec2& v) {
		vec2 ret = u;
		return ret += v;
	}

	inline vec3 operator + (const vec3& u, const vec3& v) {
		vec3 ret = u;
		return ret += v;
	}

	inline vec4 operator + (const vec4& u, const vec4& v) {
		vec4 ret = u;
		return ret += v;
	}

	inline vec2& operator /= (vec2& u, float other) {
		u.x /= other;
		u.y /= other;
		return u;
	}

	inline vec3& operator /= (vec3& u, float other) {
		u.x /= other;
		u.y /= other;
		u.z /= other;
		return u;
	}

	inline vec4& operator /= (vec4& u, float other) {
		u.x /= other;
		u.y /= other;
		u.z /= other;
		u.w /= other;
		return u;
	}

	inline vec2 operator *= (vec2& u, float other) {
		u.x *= other;
		u.y *= other;
		return u;
	}

	inline vec3 operator *= (vec3& u, float other) {
		u.x *= other;
		u.y *= other;
		u.z *= other;
		return u;
	}

	inline vec4 operator *= (vec4& u, float other) {
		u.x *= other;
		u.y *= other;
		u.z *= other;
		u.w *= other;
		return u;
	}

	inline vec2& operator -= (vec2& u, const vec2& v) {
		u.x -= v.x;
		u.y -= v.y;
		return u;
	}

	inline vec3& operator -= (vec3& u, const vec3& v) {
		u.x -= v.x;
		u.y -= v.y;
		u.z -= v.z;
		return u;
	}

	inline vec4& operator -= (vec4& u, const vec4& v) {
		u.x -= v.x;
		u.y -= v.y;
		u.z -= v.z;
		u.w -= v.w;
		return u;
	}

	inline vec2 operator -= (const vec2& u, const vec2& v) {
		return{ u.x - v.x,u.y - v.y };
	}

	inline vec3 operator -= (const vec3& u, const vec3& v) {
		return{ u.x - v.x,u.y - v.y, u.z - v.z };
	}

	inline vec4 operator -= (const vec4& u, const vec4& v) {
		return{ u.x - v.x,u.y - v.y, u.z - v.z, u.w - v.w };
	}

	inline vec2 operator * (const vec2& u, float v) {
		return{ u.x * v, u.y * v };
	}

	inline vec3 operator * (const vec3& u, float v) {
		return{ u.x * v, u.y * v, u.z * v };
	}

	inline vec4 operator * (const vec4& u, float v) {
		return{ u.x * v, u.y * v, u.z * v, u.w * v };
	}

	inline vec2 operator * (float v, const vec2& u) {
		return{ u.x * v, u.y * v };
	}

	inline vec3 operator * (float v, const vec3& u) {
		return{ u.x * v, u.y * v, u.z * v };
	}

	inline vec4 operator * (float v, const vec4& u) {
		return{ u.x * v, u.y * v, u.z * v, u.w * v };
	}

	inline vec2 operator / (const vec2& u, const float& v) {
		vec2 ret = u;
		return ret /= v;
	}

	inline vec3 operator / (const vec3& u, const float& v) {
		vec3 ret = u;
		return ret /= v;
	}

	inline vec4 operator / (const vec4& u, const float& v) {
		vec4 ret = u;
		return ret /= v;
	}

	inline float dot(const vec3& v, const vec3& u) {
		float t = 0.0f;
		for (int i = 0; i < 3; ++i) {
			t += v.data[i] * u.data[i];
		}
		return t;
	}

	inline float dot(const vec4& v, const vec4& u) {
		float t = 0.0f;
		for (int i = 0; i < 4; ++i) {
			t += v.data[i] * u.data[i];
		}
		return t;
	}

	inline float length(const vec2& v) {
		return static_cast<float>(sqrt(v.x * v.x + v.y * v.y));
	}

	inline float length(const vec3& v) {
		return static_cast<float>(sqrt(dot(v, v)));
	}

	inline float length(const vec4& v) {
		return static_cast<float>(sqrt(dot(v, v)));
	}

	inline float sqr_length(const vec2& v) {
		return v.x * v.x + v.y * v.y;
	}

	inline float sqr_length(const vec3& v) {
		return dot(v,v);
	}

	inline float sqr_length(const vec4& v) {
		return dot(v, v);
	}

	inline vec2 normalize(const vec2& u) {
		float len = length(u);
		if (len == 0.0f) {
			return { 0.0f, 0.0f };
		}
		return u / len;
	}

	inline vec3 normalize(const vec3& u) {
		float len = length(u);
		if (len == 0.0f) {
			return{ 0.0f, 0.0f, 0.0f };
		}
		return u / len;
	}

	inline vec4 normalize(const vec4& u) {
		float len = length(u);
		if (len == 0.0f) {
			return{ 0.0f, 0.0f, 0.0f, 0.0f };
		}
		return u / len;
	}
	
	inline float distance(const vec2& u, const vec2& v) {
		vec2 sub = u - v;
		return length(sub);
	}

	inline float distance(const vec3& u, const vec3& v) {
		vec3 sub = u - v;
		return length(sub);
	}

	inline float distance(const vec4& u, const vec4& v) {
		vec4 sub = u - v;
		return length(sub);
	}

	inline float sqr_distance(const vec2& u, const vec2& v) {
		vec2 sub = u - v;
		return sqr_length(sub);
	}

	inline float sqr_distance(const vec3& u, const vec3& v) {
		vec3 sub = u - v;
		return sqr_length(sub);
	}

	inline float sqr_distance(const vec4& u, const vec4& v) {
		vec4 sub = u - v;
		return sqr_length(sub);
	}
	
	inline vec3 cross(const vec3& u, const vec3& v) {
		return{
			u.y * v.z - u.z * v.y,
			u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x
		};
	}
	/*
	template<class T>
	Vector<3, T>* cross(const Vector<3, T>& u, const Vector<3, T>& v, Vector<3, T>* ret) {
		ret->x = u.y * v.z - u.z * v.y;
		ret->y = u.z * v.x - u.x * v.z;
		ret->z = u.x * v.y - u.y * v.x;
		return ret;
	}
	*/
	/*
	template<class T>
	T inline cross(const Vector<2, T>& v1, const Vector<2, T>& vec2) {
		return v1.x * vec2.y - vec2.x * v1.y;
	}
	*/
	/*
	
	const vec2 vec2_RIGHT = vec2(1, 0);
	const vec2 vec2_LEFT = vec2(-1, 0);
	const vec2 vec2_UP = vec2(0, 1);
	const vec2 vec2_DOWN = vec2(0, -1);
	const vec2 vec2_ZERO = vec2(0, 0);
	const vec2 vec2_ONE = vec2(1, 1);

	const vec3 vec3_RIGHT = vec3(1, 0, 0);
	const vec3 vec3_LEFT = vec3(-1, 0, 0);
	const vec3 vec3_UP = vec3(0, 1, 0);
	const vec3 vec3_DOWN = vec3(0, -1, 0);
	const vec3 vec3_FORWARD = vec3(0, 0, -1);
	const vec3 vec3_BACKWARD = vec3(0, 0, 1);
	const vec3 vec3_ZERO = vec3(0, 0, 0);
	const vec3 vec3_ONE = vec3(1, 1, 1);

	// ------------------------------------------------
	// Matrix
	// ------------------------------------------------
	struct matrix {

		union {
			struct {
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;

			};
			float m[4][4];
		};

		matrix();
		matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
		matrix(const float* other) {
			for (int y = 0; y < 4; ++y) {
				for (int x = 0; x < 4; ++x) {
					m[x][y] = other[x + y * 4];
				}
			}
		}
		operator float *() const { return (float *)&_11; }

		float& operator () (int a, int b) {
			return m[a][b];
		}
	};
	matrix matIdentity();

	matrix matOrthoLH(float w, float h, float zn, float zf);

	matrix matScale(const vec3& scale);

	matrix matRotationX(float angle);

	matrix matRotationY(float angle);

	matrix matRotationZ(float angle);

	matrix matRotation(const vec3& r);

	matrix matTranspose(const matrix& m);

	matrix matTranslate(const vec3& pos);

	matrix matLookAtLH(const vec3& eye, const vec3& lookAt, const vec3& up);

	matrix matPerspectiveFovLH(float fovy, float aspect, float zn, float zf);

	vec3 matTransformNormal(const vec3& v, const matrix& m);

	matrix matRotation(const vec3& v, float angle);

	matrix matInverse(const matrix& m);

	matrix operator * (const matrix& m1, const matrix& m2);

	vec3 operator * (const matrix& m, const vec3& v);

	vec4 operator * (const matrix& m, const vec4& v);
	
	template<int Size, class T>
	Vector<Size, T> operator *= (Vector<Size, T>& u, T other) {
		for (int i = 0; i < Size; ++i) {
			u.data[i] *= other;
		}
		return u;
	}

	

	template<int Size, class T>
	Vector<Size, T>& operator -= (Vector<Size, T>& u, const Vector<Size, T>& v) {
		for (int i = 0; i < Size; ++i) {
			u.data[i] -= v.data[i];
		}
		return u;
	}

	template<int Size, class T>
	Vector<Size, T> operator -= (const Vector<Size, T>& u, const Vector<Size, T>& v) {
		Vector<Size, T> r;
		for (int i = 0; i < Size; ++i) {
			r.data[i] = u.data[i] - v.data[i];
		}
		return r;
	}

	

	template<int Size, class T>
	Vector<Size, T> operator - (const Vector<Size, T>& u, const Vector<Size, T>& v) {
		Vector<Size, T> ret = u;
		return ret -= v;
	}

	template<int Size, class T>
	Vector<Size, T> operator * (const Vector<Size, T>& u, const T& v) {
		Vector<Size, T> ret = u;
		return ret *= v;
	}

	template<int Size, class T>
	Vector<Size, T> operator * (const T& v, const Vector<Size, T>& u) {
		Vector<Size, T> ret = u;
		return ret *= v;
	}

	

	

	

	template<int Size>
	Vector<Size, float>* lerp(const Vector<Size, float>& u, const Vector<Size, float>& v, float time, Vector<Size, float>* ret) {
		float norm = time;
		if (norm < 0.0f) {
			norm = 0.0f;
		}
		if (norm > 1.0f) {
			norm = 1.0f;
		}
		for (int i = 0; i < Size; ++i) {
			ret->data[i] = u.data[i] * (1.0f - norm) + v.data[i] * norm;
		}
		return ret;
	}

	template<int Size>
	Vector<Size, float> lerp(const Vector<Size, float>& u, const Vector<Size, float>& v, float time) {
		Vector<Size, float> ret;
		lerp(u, v, time, &ret);
		return ret;
	}

	template<int Size, class T>
	Vector<Size, T> vec_min(const Vector<Size, T>& u, const Vector<Size, T>& v) {
		Vector<Size, T> ret;
		for (int i = 0; i < Size; ++i) {
			if (u.data[i] <= v.data[i]) {
				ret.data[i] = u.data[i];
			}
			else {
				ret.data[i] = v.data[i];
			}
		}
		return ret;
	}

	template<int Size, class T>
	Vector<Size, T> vec_max(const Vector<Size, T>& u, const Vector<Size, T>& v) {
		Vector<Size, T> ret;
		for (int i = 0; i < Size; ++i) {
			if (u.data[i] >= v.data[i]) {
				ret.data[i] = u.data[i];
			}
			else {
				ret.data[i] = v.data[i];
			}
		}
		return ret;
	}

	template<int Size, class T>
	Vector<Size, T> clamp(const Vector<Size, T>& u, const Vector<Size, T>& min, const Vector<Size, T>& max) {
		Vector<Size, T> ret;
		for (int i = 0; i < Size; ++i) {
			ret.data[i] = u.data[i];
			if (u.data[i] > max.data[i]) {
				ret.data[i] = max.data[i];
			}
			else if (u.data[i] < min.data[i]) {
				ret.data[i] = min.data[i];
			}
		}
		return ret;
	}

	template<int Size, class T>
	void clamp(Vector<Size, T>* u, const Vector<Size, T>& min, const Vector<Size, T>& max) {
		for (int i = 0; i < Size; ++i) {
			if (u->data[i] > max.data[i]) {
				u->data[i] = max.data[i];
			}
			else if (u->data[i] < min.data[i]) {
				u->data[i] = min.data[i];
			}
		}
	}

	template<int Size>
	Vector<Size, float> saturate(const Vector<Size, float>& u) {
		return clamp(u, Vector<Size, float>(0.0f), Vector<Size, float>(1.0f));
	}

	template<int Size>
	Vector<Size, int> saturate(const Vector<Size, int>& u) {
		return clamp(u, Vector<Size, int>(0), Vector<Size, int>(1));
	}

	template<int Size, class T>
	void limit(Vector<Size, T>* v, const Vector<Size, T>& u) {
		for (int i = 0; i < Size; ++i) {
			if (v->data[i] > u.data[i]) {
				v->data[i] = u.data[i];
			}
			else if (v->data[i] < -u.data[i]) {
				v->data[i] = -u.data[i];
			}
		}
	}

	template<int Size, class T>
	Vector<Size, T> reflect(const Vector<Size, T>& u, const Vector<Size, T>& norm) {
		Vector<Size, T> ret;
		float dp = dot(u, norm);
		for (int i = 0; i < Size; ++i) {
			ret.data[i] = u.data[i] - 2.0f * dp * norm.data[i];
		}
		return ret;
	}

	inline matrix::matrix() {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				m[i][j] = 0.0f;
			}
		}
	}

	inline matrix::matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44) {
		_11 = m11;
		_12 = m12;
		_13 = m13;
		_14 = m14;
		_21 = m21;
		_22 = m22;
		_23 = m23;
		_24 = m24;
		_31 = m31;
		_32 = m32;
		_33 = m33;
		_34 = m34;
		_41 = m41;
		_42 = m42;
		_43 = m43;
		_44 = m44;
	}

	typedef struct Color {
		union {
			float values[4];
			struct {
				float r;
				float g;
				float b;
				float a;
			};
		};
		Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
		Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
		Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b), a(1.0f) {}
		Color(int _r, int _g, int _b, int _a) {
			r = static_cast<float>(_r) / 255.0f;
			g = static_cast<float>(_g) / 255.0f;
			b = static_cast<float>(_b) / 255.0f;
			a = static_cast<float>(_a) / 255.0f;
		}
		operator float* () {
			return &values[0];
		}

		operator const float* () const {
			return &values[0];
		}

	} Color;

	*/
	// ******************************************************
	//
	// Math
	//
	// ******************************************************
	inline matrix matIdentity() {
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
	}

	inline matrix matOrthoLH(float w, float h, float zn, float zf) {
		// msdn.microsoft.com/de-de/library/windows/desktop/bb204940(v=vs.85).aspx
		matrix tmp = matIdentity();
		tmp._11 = 2.0f / w;
		tmp._22 = 2.0f / h;
		tmp._33 = 1.0f / (zf - zn);
		tmp._43 = zn / (zn - zf);
		return tmp;
	}

	inline matrix operator * (const matrix& m1, const matrix& m2) {
		matrix tmp;
		tmp._11 = m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31 + m1._14 * m2._41;
		tmp._12 = m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32 + m1._14 * m2._42;
		tmp._13 = m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33 + m1._14 * m2._43;
		tmp._14 = m1._11 * m2._14 + m1._12 * m2._24 + m1._13 * m2._34 + m1._14 * m2._44;

		tmp._21 = m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31 + m1._24 * m2._41;
		tmp._22 = m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32 + m1._24 * m2._42;
		tmp._23 = m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33 + m1._24 * m2._43;
		tmp._24 = m1._21 * m2._14 + m1._22 * m2._24 + m1._23 * m2._34 + m1._24 * m2._44;

		tmp._31 = m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31 + m1._34 * m2._41;
		tmp._32 = m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32 + m1._34 * m2._42;
		tmp._33 = m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33 + m1._34 * m2._43;
		tmp._34 = m1._31 * m2._14 + m1._32 * m2._24 + m1._33 * m2._34 + m1._34 * m2._44;

		tmp._41 = m1._41 * m2._11 + m1._42 * m2._21 + m1._43 * m2._31 + m1._44 * m2._41;
		tmp._42 = m1._41 * m2._12 + m1._42 * m2._22 + m1._43 * m2._32 + m1._44 * m2._42;
		tmp._43 = m1._41 * m2._13 + m1._42 * m2._23 + m1._43 * m2._33 + m1._44 * m2._43;
		tmp._44 = m1._41 * m2._14 + m1._42 * m2._24 + m1._43 * m2._34 + m1._44 * m2._44;

		return tmp;
	}

	// -------------------------------------------------------
	// Scale matrix
	// -------------------------------------------------------
	inline matrix matScale(const vec3& scale) {
		return {
			scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, scale.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	// http://www.cprogramming.com/tutorial/3d/rotationMatrices.html
	// left hand sided
	inline matrix matRotationX(float angle) {
		float c = static_cast<float>(cos(angle));
		float s = static_cast<float>(sin(angle));
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f,    c,   -s, 0.0f,
			0.0f,    s,    c, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	inline matrix matRotationY(float angle) {
		float c = static_cast<float>(cos(angle));
		float s = static_cast<float>(sin(angle));
		return {
			   c, 0.0f,    s, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			  -s, 0.0f,    c, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}
	// FIXME: wrong direction!!!!
	inline matrix matRotationZ(float angle) {
		float c = static_cast<float>(cos(angle));
		float s = static_cast<float>(sin(angle));
		return{
			   c,   -s, 0.0f, 0.0f,
			   s,    c, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	inline matrix matRotation(const vec3& r) {
		return matRotationZ(r.z) * matRotationY(r.y) * matRotationX(r.x);
	}

	// -------------------------------------------------------
	// Transpose matrix
	// -------------------------------------------------------
	inline matrix matTranspose(const matrix& m) {
		matrix current = m;
		matrix tmp;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				tmp.m[i][j] = current.m[j][i];
			}
		}
		return tmp;
	}

	// -------------------------------------------------------
	// Translation matrix
	// -------------------------------------------------------
	inline matrix matTranslate(const vec3& pos) {
		return{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			pos.x, pos.y, pos.z, 1.0f
		};
	}

	inline matrix matLookAtLH(const vec3& eye, const vec3& lookAt, const vec3& up) {
		// see msdn.microsoft.com/de-de/library/windows/desktop/bb205342(v=vs.85).aspx
		vec3 zAxis = normalize(lookAt - eye);
		vec3 xAxis = normalize(cross(up, zAxis));
		vec3 yAxis = cross(zAxis, xAxis);
		float dox = -dot(xAxis, eye);
		float doy = -dot(yAxis, eye);
		float doz = -dot(zAxis, eye);
		return{
			xAxis.x, yAxis.x, zAxis.x, 0.0f,
			xAxis.y, yAxis.y, zAxis.y, 0.0f,
			xAxis.z, yAxis.z, zAxis.z, 0.0f,
			dox, doy, doz, 1.0f
		};
	}

	inline matrix matPerspectiveFovLH(float fovy, float aspect, float zn, float zf) {
		// msdn.microsoft.com/de-de/library/windows/desktop/bb205350(v=vs.85).aspx
		float yScale = 1.0f / static_cast<float>(tan(fovy / 2.0f));
		float xScale = yScale / aspect;
		return{
			xScale, 0.0f, 0.0f, 0.0f,
			0.0f, yScale, 0.0f, 0.0f,
			0.0f, 0.0f, zf / (zf - zn), 1.0f,
			0.0f, 0.0f, -zn*zf / (zf - zn), 0.0f
		};
	}

	inline vec3 matTransformNormal(const vec3& v, const matrix& m) {
		return {
			v.x * m._11 + v.y * m._21 + v.z * m._31,
			v.x * m._12 + v.y * m._22 + v.z * m._32,
			v.x * m._13 + v.y * m._23 + v.z * m._33 
		};
	}

	inline matrix matRotation(const vec3& v, float angle) {
		float L = (v.x * v.x + v.y * v.y + v.z * v.z);
		float u2 = v.x * v.x;
		float vec2 = v.y * v.y;
		float w2 = v.z * v.z;
		float s = static_cast<float>(sin(angle));
		float c = static_cast<float>(cos(angle));
		float LS = static_cast<float>(sqrt(L));

		matrix tmp = matIdentity();
		tmp._11 = (u2 + (vec2 + w2) * c) / L;
		tmp._12 = (v.x * v.y * (1 - c) - v.z * LS * s) / L;
		tmp._13 = (v.x * v.z * (1 - c) + v.y * LS * s) / L;
		tmp._14 = 0.0f;

		tmp._21 = (v.x * v.y * (1 - c) + v.z * LS * s) / L;
		tmp._22 = (vec2 + (u2 + w2) * c) / L;
		tmp._23 = (v.y * v.z * (1 - c) - v.x * LS * s) / L;
		tmp._24 = 0.0f;

		tmp._31 = (v.x * v.z * (1 - c) - v.y * LS * s) / L;
		tmp._32 = (v.y * v.z * (1 - c) + v.x * LS * s) / L;
		tmp._33 = (w2 + (u2 + vec2) * c) / L;
		tmp._34 = 0.0f;

		return tmp;
	}
	/*
	inline matrix matInverse(const matrix& m) {
		matrix ret;
		float tmp[12]; 
		float src[16]; 
		float det; 
		float* dst = ret;
		float* mat = m;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				src[i * 4 + j] = m.m[i][j];
			}
		}
		// transpose matrix 
		for (int i = 0; i < 4; i++) {
			src[i] = mat[i * 4];
			src[i + 4] = mat[i * 4 + 1];
			src[i + 8] = mat[i * 4 + 2];
			src[i + 12] = mat[i * 4 + 3];
		}
		// calculate pairs for first 8 elements (cofactors) 
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];
		// calculate first 8 elements (cofactors) 
		dst[0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
		dst[0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
		dst[1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
		dst[1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
		dst[2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
		dst[2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
		dst[3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
		dst[3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
		dst[4] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
		dst[4] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
		dst[5] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
		dst[5] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
		dst[6] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
		dst[6] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
		dst[7] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
		dst[7] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
		// calculate pairs for second 8 elements (cofactors) 
		tmp[0] = src[2] * src[7];
		tmp[1] = src[3] * src[6];
		tmp[2] = src[1] * src[7];
		tmp[3] = src[3] * src[5];
		tmp[4] = src[1] * src[6];
		tmp[5] = src[2] * src[5];
		tmp[6] = src[0] * src[7];
		tmp[7] = src[3] * src[4];
		tmp[8] = src[0] * src[6];
		tmp[9] = src[2] * src[4];
		tmp[10] = src[0] * src[5];
		tmp[11] = src[1] * src[4];
		// calculate second 8 elements (cofactors) 
		dst[8] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
		dst[8] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
		dst[9] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
		dst[9] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
		dst[10] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
		dst[10] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
		dst[11] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
		dst[11] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
		dst[12] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
		dst[12] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
		dst[13] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
		dst[13] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
		dst[14] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
		dst[14] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
		dst[15] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
		dst[15] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
		// calculate determinant 
		det = src[0] * dst[0] + src[1] * dst[1] + src[2] * dst[2] + src[3] * dst[3];
		// calculate matrix inverse 
		det = 1 / det;
		for (int j = 0; j < 16; j++) {
			dst[j] *= det;
		}
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				ret.m[i][j] = dst[i * 4 + j];
			}
		}
		return ret;
	}
	*/
	inline vec4 operator * (const matrix& m, const vec4& v) {
		// column mode
		//Vector4f tmp;
		//tmp.x = m._11 * v.x + m._12 * v.y + m._13 * v.z + m._14 * v.w;
		//tmp.y = m._21 * v.x + m._22 * v.y + m._23 * v.z + m._24 * v.w;
		//tmp.z = m._31 * v.x + m._32 * v.y + m._33 * v.z + m._34 * v.w;
		//tmp.w = m._41 * v.x + m._42 * v.y + m._43 * v.z + m._44 * v.w;
		//return tmp;
		// row mode
		vec4 tmp;
		tmp.x = m._11 * v.x + m._21 * v.y + m._31 * v.z + m._41 * v.w;
		tmp.y = m._12 * v.x + m._22 * v.y + m._32 * v.z + m._42 * v.w;
		tmp.z = m._13 * v.x + m._23 * v.y + m._33 * v.z + m._43 * v.w;
		tmp.w = m._14 * v.x + m._24 * v.y + m._34 * v.z + m._44 * v.w;
		return tmp;
	}

	inline vec3 operator * (const matrix& m, const vec3& v) {
		vec4 nv = { v.x, v.y, v.z, 1.0f };
		vec4 tmp = m * nv;
		return { tmp.x, tmp.y, tmp.z };
	}

}
