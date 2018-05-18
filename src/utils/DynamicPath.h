#pragma once
#include <diesel.h>
#include <ds_tweening.h>

namespace ds {

	enum PathLoopMode {
		PLM_ZERO,
		PLM_LAST,
		PLM_LOOP
	};

	template<class T>
	struct PathItem {

		float time;
		T value;
	};

	const int MAX_FLOAT_ARRAY_ITEMS = 20;

	template<class T>
	class AbstractPath {

	public:
		AbstractPath() : _count(0), _tweening(tweening::linear), _loopMode(PLM_LAST), _lastIndex(0), _lastTime(0.0f) {}
		~AbstractPath() {}

		void add(float timeStep, const T& value) {
			if (_count < 20) {
				PathItem<T>* item = &_array[_count++];
				item->time = timeStep;
				item->value = value;
			}
		}

		float normalize(float time) const {
			float normTime = time;
			if (_loopMode == PLM_ZERO) {
				float maxTime = _array[_count - 1].time;
				if (normTime > maxTime) {
					normTime = 0.0f;
				}
			}
			else if (_loopMode == PLM_LAST) {
				float maxTime = _array[_count - 1].time;
				if (normTime > maxTime) {
					normTime = maxTime;
				}
			}
			else if (_loopMode == PLM_LOOP) {
				float minTime = _array[0].time;
				float maxTime = _array[_count - 1].time;
				normTime = fmod(time, (maxTime - minTime));
			}
			return normTime;
		}

		T get(float time) {
			if (_count == 0) {
				return T(0.0f);
			}
			if (_count == 1) {
				return _array[0].value;
			}
			float normTime = normalize(time);
			// we assume that we are moving forward in time
			int start = 0;
			if (_lastIndex != -1 && _lastTime < time) {
				start = _lastIndex;
			}
			else {
				_lastIndex = -1;
				_lastTime = 0.0f;
			}
			for (int i = start; i < _count - 1; ++i) {
				PathItem<T>* current = &_array[i];
				PathItem<T>* next = &_array[i + 1];
				if (normTime >= current->time && normTime <= next->time) {
					_lastIndex = i;
					_lastTime = time;
					float t = (normTime - current->time) / (next->time - current->time);
					return tweening::interpolate(_tweening, current->value, next->value, t,1.0f);
				}
			}
			return T(0.0f);
		}

		void get(float time, T* ret) const {
			if (_count > 0) {
				if (_count == 1) {
					*ret = _array[0].value;
				}
				else {
					float normTime = normalize(time);
					for (int i = 0; i < _count - 1; ++i) {
						const PathItem<T>* current = &_array[i];
						const PathItem<T>* next = &_array[i + 1];
						if (normTime >= current->time && normTime <= next->time) {
							float t = (normTime - current->time) / (next->time - current->time);
							*ret = tweening::interpolate(_tweening, current->value, next->value, t, 1.0f);
						}
					}
				}
			}
		}

		void reset() {
			_count = 0;
		}

		const int size() const {
			return _count;
		}

		const float key(int index) const {
			return _array[index].time;
		}

		const T& value(int index) const {
			return _array[index].value;
		}

		void setInterpolationMode(const tweening::TweeningType& tweening) {
			_tweening = tweening;
		}

		void setLoopMode(const PathLoopMode& loopMode) {
			_loopMode = loopMode;
		}
	private:
		PathItem<T> _array[MAX_FLOAT_ARRAY_ITEMS];
		PathLoopMode _loopMode;
		int _count;
		int _lastIndex;
		float _lastTime;
		tweening::TweeningType _tweening;
	};

	typedef AbstractPath<float> FloatPath;
	typedef AbstractPath<ds::Color> ColorPath;
	typedef AbstractPath<ds::vec2> Vec2Path;
	typedef AbstractPath<ds::vec3> Vec3Path;

}

