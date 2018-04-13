#pragma once
//#define DS_STRING_IMPLEMENTATION

namespace ds {

	static const char NULL_CHAR = '\0';

	class string {

	public:

		typedef char* iterator;
		typedef const char* const_iterator;
		typedef char* pointer;
		typedef const char* const_pointer;
		string();
		string(const char* s);
		~string();
		void append(char c);
		void append(int i);
		void append(float f);
		void append(double d);
		void append(const_pointer txt);
		string& operator+=(const string& other);  // FIXME

		iterator begin() { return _first; }
		iterator end() { return _last; }
		const_iterator begin() const { return _first; }
		const_iterator end() const { return _last; }

		const char& operator [] (size_t n) const;
		const char& at(size_t n) const;
		const char& back() const; // FIXME
		const char& front() const { return *_first; }

		const_iterator c_str() const;
		bool is_small() const;
		size_t size() const;
		size_t capacity() const;
		bool is_empty() const;

		void resize(size_t s);
		void reserve(size_t s);
		void clear();

		//push_back
		//assign
		//insert
		//erase
		//replace
		//swap
		//pop_back

		bool find(const_pointer t, size_t offset = 0, size_t* index = 0) const;
		bool rfind(const_pointer t, size_t offset = 0, size_t* index = 0) const;
		bool find_first_of(const_pointer t, size_t offset = 0, size_t* index = 0) const;
		bool find_last_of(const_pointer t, size_t offset = 0, size_t* index = 0) const;
		bool find_last_not_of(const_pointer t, size_t offset = 0, size_t* index = 0) const;
		bool compare(const_pointer t) const;
		string substr(size_t start) const;
		string substr(size_t start, size_t end) const;

		unsigned int hash_code() const;
	private:
		void append(const char* first, const char* last);
		size_t get_length(const char* t) const;
		pointer _first;
		pointer _last;
		pointer _capacity;
		char _buffer[16];
	};

	namespace str {

		inline string value_of(int i) {
			char tmp[16];
			sprintf_s(tmp, "%d", i);
			return string(tmp);
		}

		inline string value_of(float f) {
			char tmp[16];
			sprintf_s(tmp, "%g", f);
			return string(tmp);
		}
	}

}

#ifdef DS_STRING_IMPLEMENTATION

namespace ds {

	string::string() : _first(_buffer), _last(_buffer), _capacity(_buffer + 16) {
	}

	string::string(const char* s) : _first(_buffer), _last(_buffer), _capacity(_buffer + 16) {
		size_t len = 0;
		char temp = '\0';
		if (!s) {
			s = &temp;
		}
		for (const_iterator it = s; *it; ++it) {
			++len;
		}
		append(s, s + len);
	}

	string::~string() {
		if (!is_small()) {
			delete[] _first;
		}
	}

	void string::clear() {
		_last = _first;
	}

	const char& string::operator [] (size_t n) const {
		if (_first + n < _capacity) {
			return _first[n];
		}
		return NULL_CHAR;
	}

	const char& string::at(size_t n) const {
		if (_first + n < _capacity) {
			return _first[n];
		}
		return NULL_CHAR;
	}

	bool string::is_small() const {
		return _first == _buffer;
	}

	bool string::is_empty() const {
		return _first == _last;
	}

	size_t string::size() const {
		return (size_t)(_last - _first);
	}

	size_t string::capacity() const {
		return (size_t)(_capacity - _first);
	}

	string::const_iterator string::c_str() const {
		return _first;
	}

	void string::reserve(size_t newSize) {
		if (_first + newSize + 1 >= _capacity) {
			const size_t size = (size_t)(_last - _first);
			pointer newfirst = new char[newSize + 1];
			for (pointer it = _first, newit = newfirst, end = _last; it != end; ++it, ++newit) {
				*newit = *it;
			}
			if (!is_small()) {
				delete[] _first;
			}
			_first = newfirst;
			_last = newfirst + size;
			_capacity = _first + newSize;
		}
	}

	void string::resize(size_t size) {
		reserve(size);
		for (pointer it = _last, end = _first + size + 1; it < end; ++it) {
			*it = 0;
		}
		_last = _first + size;
	}

	size_t string::get_length(const char* t) const {
		size_t len = 0;
		char temp = '\0';
		if (!t) {
			t = &temp;
		}
		for (const_iterator it = t; *it; ++it) {
			++len;
		}
		return len;
	}

	void string::append(const_pointer t) {
		size_t len = 0;
		char temp = '\0';
		if (!t) {
			t = &temp;
		}
		for (const_iterator it = t; *it; ++it) {
			++len;
		}
		append(t, t + len);
	}

	void string::append(const char* first, const char* last) {
		const size_t newsize = (size_t)((_last - _first) + (last - first) + 1);
		if (_first + newsize > _capacity) {
			reserve((newsize * 3) / 2);
		}
		for (; first != last; ++_last, ++first) {
			*_last = *first;
		}
		*_last = 0;
	}

	void string::append(char c) {
		append(&c, &c + 1);
	}

	void string::append(int i) {
		char temp[16];
		sprintf_s(temp, "%d", i);
		append(temp);
	}

	void string::append(float f) {
		char temp[16];
		sprintf_s(temp, "%g", f);
		append(temp);
	}

	bool string::find(const_pointer t, size_t offset, size_t* index) const {
		size_t tl = get_length(t);
		size_t cl = size();
		if (tl > cl) {
			return false;
		}
		char first = t[0];
		for (size_t i = offset; i <= cl - tl; ++i) {
			char current = _first[i];
			bool found = true;
			if (current == first) {
				for (unsigned j = 1; j < tl; ++j) {
					current = _first[i + j];
					char d = t[j];
					if (current != d) {
						found = false;
						break;
					}
				}
				if (found) {
					if (index != 0) {
						*index = i;
					}
					return true;
				}
			}
		}
		return false;
	}
}
#endif
