#pragma once
#include <assert.h>
#include <utility>

namespace ds {

	static const char* RESULT_SUCCESS = "Success";

	template<class T>
	class optional {

	};


	template<class T>
	class result {

	public:
		result() {
			_valid = false;
			_statusCode = 0;
			sprintf_s(_message, "%s", RESULT_SUCCESS);
			_checked = false;
		}

		explicit result(T&& t) {			
			_valid = true;
			std::swap<T>(_payload, t);
			_checked = false;
			_statusCode = 0;
			sprintf_s(_message,"%s", RESULT_SUCCESS);			
		}
		
		explicit result(const T& t) {
			_valid = true;
			_payload = t;
			_checked = false;
			_statusCode = 0;
			sprintf_s(_message, "%s", RESULT_SUCCESS);
		}
			
		result(int errorCode, const char* format, ...) {
			_checked = false;
			_statusCode = errorCode;
			va_list args;
			va_start(args, format);
			memset(_message, 0, sizeof(_message));
			int written = vsnprintf_s(_message, sizeof(_message), _TRUNCATE, format, args);
			va_end(args);
		}

		~result() {
			assert(_checked);
		}
		
		int getStatusCode() const {
			return _statusCode;
		}
	
		bool operator==(const result<T>& other) {
			return _error.code == other.getError().code;
		}
		
		bool isValid() const {
			_checked = true;
			return _valid;
		}
		
		const T& getPayload() const {
			return _payload;
		}
		
		T& getPayload() {
			return _payload;
		}

		const char* getMessage() const {
			return _message;
		}
	private:
		int _statusCode;
		char _message[256];
		T _payload;
		bool _valid;
		bool _checked;
	};


}

class Sandbox {

public:
	ds::result<int> readValues() {
		return ds::result<int>(100);
	}
};