#pragma once
#include <diesel.h>

// ----------------------------------------------------
//
// Tweakable
//
// https://www.gamedev.net/articles/programming/general-and-gameplay-programming/tweakable-constants-r2731/
//
// ----------------------------------------------------

#ifdef DEBUG
#define _TV(value) _TweakValue(__FILE__, __COUNTER__, value)
#else
#define _TV(value) value
#endif

void ReloadChangedTweakableValues();

// The following types are supported as _TV(xxx)
float _TweakValue(const char* file, size_t counter, float orgValue);
ds::vec2 _TweakValue(const char* file, size_t counter, const ds::vec2& orgValue);
ds::vec3 _TweakValue(const char* file, size_t counter, const ds::vec3& orgValue);
ds::vec4 _TweakValue(const char* file, size_t counter, const ds::vec4& orgValue);
ds::Color _TweakValue(const char* file, size_t counter, const ds::Color& orgValue);

#ifdef DS_TWEAKABLE_IMPLEMENTATION

#include <vector>
#include <windows.h>
#include <map>

struct TVar {

	enum Type {
		TV_INT,
		TV_FLOAT,
		TV_VEC2,
		TV_VEC3,
		TV_VEC4,
		TV_COLOR,
		TV_UNKNOWN
	};

	Type type;

	union {
		float fValue;
		int iValue;
		ds::vec2 v2Value;
		ds::vec3 v3Value;
		ds::vec4 v4Value;
		ds::Color cValue;
	};

	TVar(float v) : type(TV_FLOAT) {
		fValue = v;
	}

	TVar(const ds::vec2& v) : type(TV_VEC2) {
		v2Value = v;
	}

	TVar(const ds::vec3& v) : type(TV_VEC3) {
		v3Value = v;
	}

	TVar(const ds::vec4& v) : type(TV_VEC4) {
		v4Value = v;
	}

	TVar(const ds::Color& v) : type(TV_COLOR) {
		cValue = v;
	}
};

struct TweakableFile {
	const char* fileName;
	FILETIME  loadTime;
};

typedef std::pair<int, int> TVKey;
typedef std::map<TVKey, TVar*> TVMap;
typedef std::vector<TweakableFile> TVFiles;

struct TweakableContext {
	TVFiles files;
	TVMap map;
};

static TweakableContext _tweakableContext;

const int FNV_Prime = 0x01000193; //   16777619
const int FNV_Seed = 0x811C9DC5; // 2166136261

// ------------------------------------------------------------------
// internal method to generate hash
// ------------------------------------------------------------------
static inline int tv__fnv1a(const char* text) {
	const unsigned char* ptr = (const unsigned char*)text;
	int hash = FNV_Seed;
	while (*ptr) {
		hash = (*ptr++ ^ hash) * FNV_Prime;
	}
	return hash;
}

static FILETIME tv__getFileModTime(const char* fileName) {
	FILETIME ftCreate, ftAccess, ftWrite;
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
	CloseHandle(hFile);
	return ftWrite;
}

static TVar* tv__addTweakable(const char* filename, size_t counter, TVar *twk) {
	bool found = false;
	for (TVFiles::iterator ti = _tweakableContext.files.begin();
	ti != _tweakableContext.files.end(); ++ti) {
		if (ti->fileName == filename) {
			found = true;
			break;
		}
	}
	if (!found) {
		DBG_LOG("=> adding tweakable file: %s", filename);
		TweakableFile srcFile;
		srcFile.fileName = filename;
		// use the initial modification time		
		srcFile.loadTime = tv__getFileModTime(filename);
		_tweakableContext.files.push_back(srcFile);
	}

	// add to the tweakables
	_tweakableContext.map[TVKey(tv__fnv1a(filename), counter)] = twk;
	return twk;
}

static TVar* tv__findTweakValue(const char *file, size_t counter) {
	TVMap::iterator fi = _tweakableContext.map.find(TVKey(tv__fnv1a(file), counter));
	if (fi != _tweakableContext.map.end()) {
		return (*fi).second;
	}
	else {
		return 0;
	}
}

// ------------------------------------------------------------------
// is numeric
// ------------------------------------------------------------------
static bool tv__is_numeric(const char c) {
	return ((c >= '0' && c <= '9'));
}

// ------------------------------------------------------------------
// is whitespace
// ------------------------------------------------------------------
static bool tv__is_whitespace(const char c) {
	if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
		return true;
	}
	return false;
}

// ------------------------------------------------------------------
// strtof
// ------------------------------------------------------------------
static float tv__strtof(const char* p, char** endPtr) {
	while (tv__is_whitespace(*p)) {
		++p;
	}
	float sign = 1.0f;
	if (*p == '-') {
		sign = -1.0f;
		++p;
	}
	else if (*p == '+') {
		++p;
	}
	float value = 0.0f;
	while (tv__is_numeric(*p)) {
		value *= 10.0f;
		value = value + (*p - '0');
		++p;
	}
	if (*p == '.') {
		++p;
		float dec = 1.0f;
		float frac = 0.0f;
		while (tv__is_numeric(*p)) {
			frac *= 10.0f;
			frac = frac + (*p - '0');
			dec *= 10.0f;
			++p;
		}
		value = value + (frac / dec);
	}
	if (*p == 'E' || *p == 'e') {
		++p;
		float nextSign = 1.0f;
		if (*p == '-') {
			nextSign = -1.0f;
			++p;
		}
		else if (*p == '+') {
			++p;
		}
		float exp = 0.0f;
		while (tv__is_numeric(*p)) {
			exp *= 10.0f;
			exp = exp + (*p - '0');
			++p;
		}
		exp *= nextSign;
		value = value * pow(10.0f, exp);
	}
	if (endPtr) {
		*endPtr = (char *)(p);
	}
	return value * sign;
}

static void tv__extract(char* str, float* values, int max) {
	char* next = str;
	for (int i = 0; i < max; ++i) {
		values[i] = tv__strtof(next, &next);
		while (!tv__is_numeric(*next)) {
			++next;
		}
	}
}

static void tv__reloadTweakableFile(const char* srcFile) {
	size_t counter = 0;
	FILE *fp = fopen(srcFile, "rt");
	char line[2048], strval[512];
	while (!feof(fp)) {
		fgets(line, 2048, fp);
		char *ch = line;
		char *comment = strstr(line, "//");
		if (comment) {
			*comment = 0;
		}
		while ((ch = strstr(ch, "_T" "V("))) {
			ch += 4; // skip the _TV( value
			char *chend = strstr(ch, ")");
			if (chend) {
				char* t = strstr(ch, "(");
				if (t) {
					ch = t + 1;
				}
				strncpy(strval, ch, chend - ch);
				strval[chend - ch] = '\0';
				ch = chend;
				TVar *tv = tv__findTweakValue(srcFile, counter);
				if (tv) {
					if (tv->type == TVar::TV_INT) {
						tv->iValue = atoi(strval);
					}
					else if (tv->type == TVar::TV_FLOAT) {
						tv->fValue = atof(strval);
					}
					else if (tv->type == TVar::TV_VEC2) {
						float f[2];
						tv__extract(strval, f, 2);
						tv->v2Value = ds::vec2(f[0], f[1]);
					}
					else if (tv->type == TVar::TV_VEC3) {
						float f[3];
						tv__extract(strval, f, 3);
						tv->v3Value = ds::vec3(f[0], f[1], f[2]);
					}
					else if (tv->type == TVar::TV_VEC4) {
						float f[4];
						tv__extract(strval, f, 4);
						tv->v4Value = ds::vec4(f[0], f[1], f[2], f[3]);
					}
					else if (tv->type == TVar::TV_COLOR) {
						float f[4];
						tv__extract(strval, f, 4);
						tv->cValue = ds::Color(f[0], f[1], f[2], f[3]);
					}
				}
			}
			else {
				break;
			}
			counter++;
		}
	}
	fclose(fp);
}


void ReloadChangedTweakableValues() {
#ifdef DEBUG
	for (int i = 0; i < _tweakableContext.files.size(); ++i) {
		TweakableFile& file = _tweakableContext.files[i];
		FILETIME current = tv__getFileModTime(file.fileName);
		int ret = CompareFileTime(&file.loadTime, &current);
		if (ret == -1) {			
			tv__reloadTweakableFile(file.fileName);
			file.loadTime = current;
		}

	}
#endif
}

float _TweakValue(const char* file, size_t counter, float orgValue) {
	TVar* tv = tv__findTweakValue(file, counter);
	if (tv) {
		return tv->fValue;
	}
	else {
		TVar *twk = new TVar(orgValue);
		return tv__addTweakable(file, counter, twk)->fValue;
	}
}

ds::vec2 _TweakValue(const char* file, size_t counter, const ds::vec2& orgValue) {
	TVar* tv = tv__findTweakValue(file, counter);
	if (tv) {
		return tv->v2Value;
	}
	else {
		TVar *twk = new TVar(orgValue);
		return tv__addTweakable(file, counter, twk)->v2Value;
	}
}

ds::vec3 _TweakValue(const char* file, size_t counter, const ds::vec3& orgValue) {
	TVar* tv = tv__findTweakValue(file, counter);
	if (tv) {
		return tv->v3Value;
	}
	else {
		TVar *twk = new TVar(orgValue);
		return tv__addTweakable(file, counter, twk)->v3Value;
	}
}

ds::vec4 _TweakValue(const char* file, size_t counter, const ds::vec4& orgValue) {
	TVar* tv = tv__findTweakValue(file, counter);
	if (tv) {
		return tv->v4Value;
	}
	else {
		TVar *twk = new TVar(orgValue);
		return tv__addTweakable(file, counter, twk)->v4Value;
	}
}

ds::Color _TweakValue(const char* file, size_t counter, const ds::Color& orgValue) {
	TVar* tv = tv__findTweakValue(file, counter);
	if (tv) {
		return tv->cValue;
	}
	else {
		TVar *twk = new TVar(orgValue);
		return tv__addTweakable(file, counter, twk)->cValue;
	}
}

#endif


